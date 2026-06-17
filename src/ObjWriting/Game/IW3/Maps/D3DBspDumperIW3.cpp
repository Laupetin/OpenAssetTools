#include "D3DBspDumperIW3.h"

#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/Maps/D3DBspCommonIW3.h"
#include "Utils/StreamUtils.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace IW3;

namespace
{
    using enum IW3::d3dbsp::LumpType;

    struct BspLump
    {
        IW3::d3dbsp::LumpType id;
        std::vector<std::byte> data;
    };

    struct IntBounds
    {
        int32_t mins[3];
        int32_t maxs[3];
    };

    struct ReflectionProbeColorCorrection
    {
        double blackLevel;
        double whiteLevel;
        double gamma;
        double saturation;
    };

    constexpr auto REFLECTION_PROBE_SIZE = 64uz;
    constexpr auto REFLECTION_PROBE_PIXEL_SIZE = 4uz;
    constexpr auto REFLECTION_PROBE_MIP_COUNT = 7uz;
    constexpr auto REFLECTION_PROBE_RAW_DATA_SIZE = 0x1FFF8uz;
    constexpr auto REFLECTION_PROBE_NAME_SIZE = 64uz;
    constexpr auto LIGHTMAP_PRIMARY_RAW_PAGE_SIZE = 0x100000uz;
    constexpr auto LIGHTMAP_SECONDARY_RAW_PAGE_SIZE = 0x200000uz;
    constexpr auto LIGHTMAP_PRIMARY_RAW_WIDTH = 1024u;
    constexpr auto LIGHTMAP_PRIMARY_RAW_HEIGHT = 1024u;
    constexpr auto LIGHTMAP_SECONDARY_RAW_WIDTH = 512u;
    constexpr auto LIGHTMAP_SECONDARY_RAW_HEIGHT = 1024u;
    constexpr auto LIGHTMAP_SECONDARY_HALF_HEIGHT = LIGHTMAP_SECONDARY_RAW_HEIGHT / 2u;
    constexpr auto LIGHTMAP_SECONDARY_PIXEL_SIZE = 4u;
    constexpr auto SKY_LIGHTMAP_INDEX = 31u;
    constexpr auto PI = 3.14159265358979323846;
    constexpr auto INVALID_STATIC_MODEL_INDEX = std::numeric_limits<size_t>::max();

    // Matches the stock raw/reflections/reflections.csv "default" row.
    constexpr auto DEFAULT_REFLECTION_PROBE_CORRECTION = ReflectionProbeColorCorrection{0.3, 0.7, 1.2, 0.5};

    struct LightmapPageLayout
    {
        std::vector<unsigned> firstRawPage;
        std::vector<unsigned> pageCount;
        std::vector<unsigned> wideCount;
        std::vector<unsigned> highCount;
        std::vector<std::vector<unsigned>> rawPageForPackedSlot;
        std::vector<std::vector<unsigned>> packedSlotForRawPage;
        unsigned rawPageCount;
    };

    struct SurfaceLightmapRemap
    {
        uint8_t rawLightmapIndex;
        unsigned lightmapIndex;
        unsigned packedSlot;
        unsigned wideCount;
        unsigned highCount;
    };

    struct VertexLightmapRemap
    {
        unsigned packedSlot;
        unsigned wideCount;
        unsigned highCount;
        bool valid;
    };

    [[nodiscard]] size_t PositiveCount(const int value)
    {
        return value > 0 ? static_cast<size_t>(value) : 0uz;
    }

    [[nodiscard]] size_t PositiveCount(const unsigned value)
    {
        return static_cast<size_t>(value);
    }

    [[nodiscard]] std::string_view BaseName(const char* value)
    {
        if (!value)
            return {};

        auto result = std::string_view(value);
        const auto lastSlash = result.find_last_of("/\\");
        if (lastSlash != std::string_view::npos)
            result.remove_prefix(lastSlash + 1uz);

        // A leading comma marks a fastfile reference asset. Raw BSP material
        // names store the target name, so ",$default" must match "$default".
        if (result.starts_with(','))
            result.remove_prefix(1uz);

        return result;
    }

    [[nodiscard]] bool MaterialNameMatches(const std::string_view rawName, const std::string_view assetName)
    {
        if (rawName == assetName)
            return true;

        // Runtime default material assets use explicit names, but raw d3dbsp stores the shared "$default" material.
        return rawName == "$default" && (assetName == "$default2d" || assetName == "$default3d");
    }

    template<typename T> void Append(std::vector<std::byte>& out, const T& value)
    {
        const auto* bytes = reinterpret_cast<const std::byte*>(&value);
        out.insert(out.end(), bytes, bytes + sizeof(T));
    }

    [[nodiscard]] uint8_t ClampToByte(const int value)
    {
        return static_cast<uint8_t>(std::clamp(value, 0, 255));
    }

    [[nodiscard]] uint8_t Byte(const char value)
    {
        return static_cast<uint8_t>(static_cast<unsigned char>(value));
    }

    void AppendBytes(std::vector<std::byte>& out, const void* data, const size_t size)
    {
        if (!data || size == 0)
            return;

        const auto* bytes = static_cast<const std::byte*>(data);
        out.insert(out.end(), bytes, bytes + size);
    }

    [[nodiscard]] std::vector<std::byte> RawBytes(const void* data, const size_t size)
    {
        std::vector<std::byte> out;
        if (data && size > 0)
            out.reserve(size);

        AppendBytes(out, data, size);
        return out;
    }

    void WriteBytesAt(std::vector<std::byte>& out, const size_t offset, const void* data, const size_t size)
    {
        if (!data || size == 0)
            return;

        assert(offset + size <= out.size());
        const auto* bytes = static_cast<const std::byte*>(data);
        std::copy_n(bytes, size, out.data() + offset);
    }

    template<typename T> void WriteAt(std::vector<std::byte>& out, const size_t offset, const T& value)
    {
        WriteBytesAt(out, offset, &value, sizeof(T));
    }

    void MarkMaterialUsedByBrush(std::vector<uint8_t>& usedMaterials, const unsigned materialIndex)
    {
        if (materialIndex < usedMaterials.size())
            usedMaterials[materialIndex] = 1u;
    }

    void MarkMaterialUsedByBrush(std::vector<uint8_t>& usedMaterials, const int materialIndex)
    {
        if (materialIndex >= 0)
            MarkMaterialUsedByBrush(usedMaterials, static_cast<unsigned>(materialIndex));
    }

    [[nodiscard]] std::vector<uint8_t> BuildBrushMaterialUsage(const clipMap_t& clipMap)
    {
        std::vector<uint8_t> usedMaterials(PositiveCount(clipMap.numMaterials), 0u);

        for (auto brushIndex = 0uz; brushIndex < clipMap.numBrushes; brushIndex++)
        {
            const auto& brush = clipMap.brushes[brushIndex];

            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                for (auto side = 0uz; side < 2uz; side++)
                    MarkMaterialUsedByBrush(usedMaterials, brush.axialMaterialNum[side][axis]);
            }

            for (auto sideIndex = 0uz; sideIndex < brush.numsides; sideIndex++)
                MarkMaterialUsedByBrush(usedMaterials, brush.sides[sideIndex].materialNum);
        }

        return usedMaterials;
    }

    [[nodiscard]] int ReconstructRawMaterialContents(const int contents, const bool usedByBrush)
    {
        if (!usedByBrush || contents == 0)
            return contents;

        // linker_pc loads BSP material contents as rawContents & 0xdffffffb.
        // That also strips a raw decal bit (0x4), but the important collision
        // bit here is the raw-only 0x20000000 brush marker. If we write the
        // already-masked runtime value back out, relinking the dumped BSP loses
        // solid/clip contents because brush contents are recomputed from this
        // material table.
        return static_cast<int>(static_cast<unsigned>(contents) | IW3::d3dbsp::RAW_BSP_BRUSH_CONTENT_MARKER);
    }

    [[nodiscard]] const GfxImageLoadDef* LoadDefForTexture(const GfxTexture* textures, const size_t index)
    {
        return textures ? textures[index].loadDef : nullptr;
    }

    [[nodiscard]] const GfxImageLoadDef* LoadDefForImage(const GfxImage* image)
    {
        return image ? image->texture.loadDef : nullptr;
    }

    [[nodiscard]] const char* NameOf(const XModel* value)
    {
        if (!value || !value->name)
            return nullptr;

        // A leading comma marks a fastfile reference asset. Raw map entity text
        // needs the real xmodel name, otherwise Radiant looks for ",model".
        return value->name[0] == ',' ? &value->name[1] : value->name;
    }

    template<typename T> [[nodiscard]] bool TryPointerIndex(const T* base, const size_t count, const T* value, size_t& index)
    {
        if (!base || !value || count == 0)
            return false;

        const auto baseAddress = reinterpret_cast<uintptr_t>(base);
        const auto valueAddress = reinterpret_cast<uintptr_t>(value);
        const auto endAddress = baseAddress + count * sizeof(T);

        if (valueAddress < baseAddress || valueAddress >= endAddress)
            return false;

        const auto offset = valueAddress - baseAddress;
        if (offset % sizeof(T) != 0)
            return false;

        index = offset / sizeof(T);
        return true;
    }

    template<typename T> [[nodiscard]] size_t PointerIndex(const T* base, const size_t count, const T* value)
    {
        auto index = 0uz;
        return TryPointerIndex(base, count, value, index) ? index : 0uz;
    }

    [[nodiscard]] size_t FindLeafBrushRange(
        const LeafBrush* leafBrushes, const size_t leafBrushCount, const LeafBrush* nodeBrushes, const size_t nodeBrushCount, const size_t searchStart)
    {
        if (!leafBrushes || !nodeBrushes || nodeBrushCount == 0 || nodeBrushCount > leafBrushCount)
            return leafBrushCount;

        const auto matchAt = [leafBrushes, nodeBrushes, nodeBrushCount](const size_t index)
        {
            return std::equal(nodeBrushes, nodeBrushes + nodeBrushCount, leafBrushes + index);
        };

        for (auto i = std::min(searchStart, leafBrushCount); i + nodeBrushCount <= leafBrushCount; i++)
        {
            if (matchAt(i))
                return i;
        }

        // Fall back to a full scan for unusual cases where the raw leafbrush
        // ranges are not monotonic. The normal linker layout is monotonic, so
        // starting at the running cursor avoids picking earlier duplicate
        // one-brush ranges when OAT-built fastfiles contain copied node arrays.
        for (auto i = 0uz; i < std::min(searchStart, leafBrushCount); i++)
        {
            if (i + nodeBrushCount <= leafBrushCount && matchAt(i))
                return i;
        }

        return leafBrushCount;
    }

    struct LeafBrushRange
    {
        size_t first = std::numeric_limits<size_t>::max();
        size_t end = 0uz;
        bool valid = false;
    };

    void IncludeLeafBrushRange(LeafBrushRange& range, const size_t first, const size_t count)
    {
        if (count == 0uz)
            return;

        range.first = std::min(range.first, first);
        range.end = std::max(range.end, first + count);
        range.valid = true;
    }

    [[nodiscard]] bool CollectLeafBrushRange_r(const clipMap_t& clipMap, const size_t nodeIndex, LeafBrushRange& range, const size_t depth = 0uz)
    {
        if (!clipMap.leafbrushNodes || nodeIndex >= clipMap.leafbrushNodesCount || depth > clipMap.leafbrushNodesCount)
            return false;

        const auto& node = clipMap.leafbrushNodes[nodeIndex];
        if (node.leafBrushCount > 0)
        {
            auto brushesIndex = 0uz;
            if (!TryPointerIndex(clipMap.leafbrushes, clipMap.numLeafBrushes, node.data.leaf.brushes, brushesIndex))
                return false;

            IncludeLeafBrushRange(range, brushesIndex, static_cast<size_t>(node.leafBrushCount));
            return true;
        }

        if (node.leafBrushCount < 0 && !CollectLeafBrushRange_r(clipMap, nodeIndex + 1uz, range, depth + 1uz))
            return false;

        for (auto side = 0uz; side < 2uz; side++)
        {
            const auto childOffset = node.data.children.childOffset[side];
            if (childOffset == 0u || !CollectLeafBrushRange_r(clipMap, nodeIndex + childOffset, range, depth + 1uz))
                return false;
        }

        return true;
    }

    [[nodiscard]] bool CollectLeafBrushes_r(const clipMap_t& clipMap, const size_t nodeIndex, std::vector<LeafBrush>& brushes, const size_t depth = 0uz)
    {
        if (!clipMap.leafbrushNodes || nodeIndex >= clipMap.leafbrushNodesCount || depth > clipMap.leafbrushNodesCount)
            return false;

        const auto& node = clipMap.leafbrushNodes[nodeIndex];
        if (node.leafBrushCount > 0)
        {
            if (!node.data.leaf.brushes)
                return false;

            brushes.insert(brushes.end(), node.data.leaf.brushes, node.data.leaf.brushes + node.leafBrushCount);
            return true;
        }

        if (node.leafBrushCount < 0 && !CollectLeafBrushes_r(clipMap, nodeIndex + 1uz, brushes, depth + 1uz))
            return false;

        for (auto side = 0uz; side < 2uz; side++)
        {
            const auto childOffset = node.data.children.childOffset[side];
            if (childOffset == 0u || !CollectLeafBrushes_r(clipMap, nodeIndex + childOffset, brushes, depth + 1uz))
                return false;
        }

        return true;
    }

    [[nodiscard]] std::vector<std::byte> BuildMaterials(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(clipMap.numMaterials) * 72uz);
        const auto brushMaterialUsage = BuildBrushMaterialUsage(clipMap);

        for (auto i = 0uz; i < clipMap.numMaterials; i++)
        {
            const auto& material = clipMap.materials[i];
            const auto contentFlags = ReconstructRawMaterialContents(material.contentFlags, brushMaterialUsage[i] != 0u);
            AppendBytes(out, material.material, sizeof(material.material));
            Append(out, material.surfaceFlags);
            Append(out, contentFlags);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildPlanes(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(PositiveCount(clipMap.planeCount) * 16uz);

        for (auto i = 0uz; i < PositiveCount(clipMap.planeCount); i++)
        {
            const auto& plane = clipMap.planes[i];
            AppendBytes(out, plane.normal, sizeof(plane.normal));
            Append(out, plane.dist);
        }

        return out;
    }

    [[nodiscard]] uint32_t PlaneIndex(const clipMap_t& clipMap, const cplane_s* plane)
    {
        return static_cast<uint32_t>(PointerIndex(clipMap.planes, PositiveCount(clipMap.planeCount), plane));
    }

    [[nodiscard]] std::vector<std::byte> BuildBrushSideData(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;

        for (auto brushIndex = 0uz; brushIndex < clipMap.numBrushes; brushIndex++)
        {
            const auto& brush = clipMap.brushes[brushIndex];

            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                for (auto side = 0uz; side < 2uz; side++)
                {
                    const auto dist = side == 0uz ? brush.mins[axis] : brush.maxs[axis];
                    const auto materialIndex = static_cast<uint32_t>(brush.axialMaterialNum[side][axis]);
                    Append(out, dist);
                    Append(out, materialIndex);
                }
            }

            for (auto sideIndex = 0uz; sideIndex < brush.numsides; sideIndex++)
            {
                const auto& side = brush.sides[sideIndex];
                Append(out, PlaneIndex(clipMap, side.plane));
                Append(out, side.materialNum);
            }
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildBrushEdgeCounts(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;

        for (auto brushIndex = 0uz; brushIndex < clipMap.numBrushes; brushIndex++)
        {
            const auto& brush = clipMap.brushes[brushIndex];

            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                for (auto side = 0uz; side < 2uz; side++)
                    Append(out, brush.edgeCount[side][axis]);
            }

            for (auto sideIndex = 0uz; sideIndex < brush.numsides; sideIndex++)
                Append(out, brush.sides[sideIndex].edgeCount);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildBrushEdges(const clipMap_t& clipMap)
    {
        return RawBytes(clipMap.brushEdges, static_cast<size_t>(clipMap.numBrushEdges) * sizeof(cbrushedge_t));
    }

    [[nodiscard]] std::vector<std::byte> BuildBrushHeaders(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(clipMap.numBrushes) * 4uz);

        for (auto brushIndex = 0uz; brushIndex < clipMap.numBrushes; brushIndex++)
        {
            const auto& brush = clipMap.brushes[brushIndex];
            const auto sideCount = static_cast<uint16_t>(brush.numsides + 6u);
            const auto materialIndex = static_cast<uint16_t>(brush.axialMaterialNum[0][0]);
            Append(out, sideCount);
            Append(out, materialIndex);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildClipLeafs(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(clipMap.numLeafs) * 24uz);

        auto runningFirstLeafBrush = 0;

        for (auto leafIndex = 0uz; leafIndex < clipMap.numLeafs; leafIndex++)
        {
            const auto& leaf = clipMap.leafs[leafIndex];
            const auto* leafBrushNode = leaf.leafBrushNode >= 0 && static_cast<unsigned>(leaf.leafBrushNode) < clipMap.leafbrushNodesCount
                                            ? &clipMap.leafbrushNodes[leaf.leafBrushNode]
                                            : nullptr;
            auto leafBrushCount = 0;
            auto firstLeafBrush = runningFirstLeafBrush;
            if (leafBrushNode)
            {
                auto recoveredRange = LeafBrushRange{};

                // linker_pc partitions each raw leaf brush range into a small
                // runtime tree. The raw BSP still wants the original contiguous
                // range, so flatten terminal leaf nodes back to their min/max
                // span inside clipMap.leafbrushes.
                if (CollectLeafBrushRange_r(clipMap, static_cast<size_t>(leaf.leafBrushNode), recoveredRange) && recoveredRange.valid)
                {
                    firstLeafBrush = static_cast<int32_t>(recoveredRange.first);
                    leafBrushCount = static_cast<int32_t>(recoveredRange.end - recoveredRange.first);
                }
                else
                {
                    std::vector<LeafBrush> recoveredBrushes;
                    if (CollectLeafBrushes_r(clipMap, static_cast<size_t>(leaf.leafBrushNode), recoveredBrushes) && !recoveredBrushes.empty())
                    {
                        const auto brushesIndex = FindLeafBrushRange(clipMap.leafbrushes,
                                                                     clipMap.numLeafBrushes,
                                                                     recoveredBrushes.data(),
                                                                     recoveredBrushes.size(),
                                                                     static_cast<size_t>(runningFirstLeafBrush));

                        if (brushesIndex < clipMap.numLeafBrushes)
                        {
                            firstLeafBrush = static_cast<int32_t>(brushesIndex);
                            leafBrushCount = static_cast<int32_t>(recoveredBrushes.size());
                        }
                    }
                }
            }

            // The raw leaf record carries the cluster/cell assignment even for
            // leaves that also reference brushes. Recomputing this from brush
            // count makes valid source leaves look solid to Radiant.
            const auto cluster = static_cast<int32_t>(leaf.cluster);
            const auto cellIndex = cluster >= 0 ? 0 : -1;

            Append(out, cluster);
            Append(out, static_cast<int32_t>(leaf.firstCollAabbIndex));
            Append(out, static_cast<int32_t>(leaf.collAabbCount));
            Append(out, firstLeafBrush);
            Append(out, leafBrushCount);
            Append(out, cellIndex);

            runningFirstLeafBrush = firstLeafBrush + leafBrushCount;
        }

        return out;
    }

    [[nodiscard]] int32_t BoundsMinToInt(const float value)
    {
        return static_cast<int32_t>(std::floor(value)) - 8;
    }

    [[nodiscard]] int32_t BoundsMaxToInt(const float value)
    {
        return static_cast<int32_t>(std::ceil(value)) + 8;
    }

    [[nodiscard]] IntBounds RootClipNodeBounds(const clipMap_t& clipMap, const GfxWorld& world)
    {
        const auto* mins = world.modelCount > 0 && world.models          ? world.models[0].bounds[0]
                           : clipMap.numSubModels > 0 && clipMap.cmodels ? clipMap.cmodels[0].mins
                                                                         : nullptr;
        const auto* maxs = world.modelCount > 0 && world.models          ? world.models[0].bounds[1]
                           : clipMap.numSubModels > 0 && clipMap.cmodels ? clipMap.cmodels[0].maxs
                                                                         : nullptr;

        IntBounds result{};
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            result.mins[axis] = mins ? BoundsMinToInt(mins[axis]) : 0;
            result.maxs[axis] = maxs ? BoundsMaxToInt(maxs[axis]) : 0;
        }

        return result;
    }

    [[nodiscard]] unsigned PlaneAxis(const cplane_s& plane)
    {
        if (plane.type <= 2)
            return plane.type;

        auto axis = 0u;
        auto largestComponent = std::abs(plane.normal[0]);
        for (auto i = 1u; i < 3u; i++)
        {
            const auto component = std::abs(plane.normal[i]);
            if (component > largestComponent)
            {
                largestComponent = component;
                axis = i;
            }
        }

        return axis;
    }

    void AssignClipNodeBounds(
        const clipMap_t& clipMap, std::vector<IntBounds>& bounds, std::vector<bool>& visited, const int nodeIndex, const IntBounds& nodeBounds)
    {
        if (nodeIndex < 0 || static_cast<size_t>(nodeIndex) >= bounds.size() || visited[nodeIndex])
            return;

        visited[nodeIndex] = true;
        bounds[nodeIndex] = nodeBounds;

        const auto& node = clipMap.nodes[nodeIndex];
        const auto planeIndex = PointerIndex(clipMap.planes, PositiveCount(clipMap.planeCount), node.plane);
        if (planeIndex >= PositiveCount(clipMap.planeCount))
            return;

        const auto& plane = clipMap.planes[planeIndex];
        const auto axis = PlaneAxis(plane);
        const auto dist = static_cast<int32_t>(std::lround(plane.dist));

        auto frontBounds = nodeBounds;
        auto backBounds = nodeBounds;
        frontBounds.mins[axis] = dist;
        backBounds.maxs[axis] = dist;

        AssignClipNodeBounds(clipMap, bounds, visited, node.children[0], frontBounds);
        AssignClipNodeBounds(clipMap, bounds, visited, node.children[1], backBounds);
    }

    [[nodiscard]] std::vector<std::byte> BuildClipNodes(const clipMap_t& clipMap, const GfxWorld& world)
    {
        const auto nodeCount = PositiveCount(clipMap.numNodes);
        std::vector<IntBounds> bounds(nodeCount);
        std::vector<bool> visited(nodeCount);

        if (nodeCount > 0)
            AssignClipNodeBounds(clipMap, bounds, visited, 0, RootClipNodeBounds(clipMap, world));

        std::vector<std::byte> out;
        out.reserve(nodeCount * 36uz);

        for (auto nodeIndex = 0uz; nodeIndex < nodeCount; nodeIndex++)
        {
            const auto& node = clipMap.nodes[nodeIndex];
            const auto planeIndex = static_cast<int32_t>(PointerIndex(clipMap.planes, PositiveCount(clipMap.planeCount), node.plane));
            Append(out, planeIndex);
            Append(out, static_cast<int32_t>(node.children[0]));
            Append(out, static_cast<int32_t>(node.children[1]));
            AppendBytes(out, bounds[nodeIndex].mins, sizeof(bounds[nodeIndex].mins));
            AppendBytes(out, bounds[nodeIndex].maxs, sizeof(bounds[nodeIndex].maxs));
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLeafBrushes(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(clipMap.numLeafBrushes) * sizeof(uint32_t));

        for (auto i = 0uz; i < clipMap.numLeafBrushes; i++)
        {
            const auto value = static_cast<uint32_t>(clipMap.leafbrushes[i]);
            Append(out, value);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionVerts(const clipMap_t& clipMap)
    {
        return RawBytes(clipMap.verts, static_cast<size_t>(clipMap.vertCount) * sizeof(vec3_t));
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionTriIndices(const clipMap_t& clipMap)
    {
        return RawBytes(clipMap.triIndices, PositiveCount(clipMap.triCount) * 3uz * sizeof(uint16_t));
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionTriEdgeIsWalkable(const clipMap_t& clipMap)
    {
        const auto size = ((PositiveCount(clipMap.triCount) * 3uz + 31uz) / 32uz) * sizeof(uint32_t);
        return RawBytes(clipMap.triEdgeIsWalkable, size);
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionBorders(const clipMap_t& clipMap)
    {
        return RawBytes(clipMap.borders, PositiveCount(clipMap.borderCount) * sizeof(CollisionBorder));
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionPartitions(const clipMap_t& clipMap)
    {
        std::vector<std::byte> out;
        out.reserve(PositiveCount(clipMap.partitionCount) * 12uz);

        for (auto i = 0uz; i < PositiveCount(clipMap.partitionCount); i++)
        {
            const auto& partition = clipMap.partitions[i];
            const uint8_t padding[2]{};
            const auto firstTri = partition.firstTri;
            const auto borderIndex = static_cast<uint32_t>(PointerIndex(clipMap.borders, PositiveCount(clipMap.borderCount), partition.borders));

            AppendBytes(out, padding, sizeof(padding));
            Append(out, partition.triCount);
            Append(out, partition.borderCount);
            Append(out, firstTri);
            Append(out, borderIndex);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildCollisionAabbTrees(const clipMap_t& clipMap)
    {
        return RawBytes(clipMap.aabbTrees, PositiveCount(clipMap.aabbTreeCount) * sizeof(CollisionAabbTree));
    }

    [[nodiscard]] size_t LightGridRowCount(const GfxLightGrid& lightGrid)
    {
        if (lightGrid.rowAxis >= 3u || lightGrid.maxs[lightGrid.rowAxis] < lightGrid.mins[lightGrid.rowAxis])
            return 0uz;

        return static_cast<size_t>(lightGrid.maxs[lightGrid.rowAxis] - lightGrid.mins[lightGrid.rowAxis] + 1u);
    }

    [[nodiscard]] std::vector<std::byte> BuildLightGridHeader(const GfxWorld& world)
    {
        const auto& lightGrid = world.lightGrid;
        std::vector<std::byte> out;

        AppendBytes(out, lightGrid.mins, sizeof(lightGrid.mins));
        AppendBytes(out, lightGrid.maxs, sizeof(lightGrid.maxs));
        Append(out, lightGrid.rowAxis);
        Append(out, lightGrid.colAxis);
        AppendBytes(out, lightGrid.rowDataStart, LightGridRowCount(lightGrid) * sizeof(uint16_t));

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLightGridEntries(const GfxWorld& world)
    {
        return RawBytes(world.lightGrid.entries, static_cast<size_t>(world.lightGrid.entryCount) * sizeof(GfxLightGridEntry));
    }

    [[nodiscard]] std::vector<std::byte> BuildLightGridColors(const GfxWorld& world)
    {
        std::vector<std::byte> out;
        auto colorCount = static_cast<size_t>(world.lightGrid.colorCount);

        // The linker appends a runtime fallback color set. It is not present in stock test.d3dbsp.
        if (colorCount > 0)
            colorCount--;

        AppendBytes(out, world.lightGrid.colors, colorCount * sizeof(GfxLightGridColors));
        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLightGridRawRows(const GfxWorld& world)
    {
        return RawBytes(world.lightGrid.rawRowData, world.lightGrid.rawRowDataSize);
    }

    [[nodiscard]] uint16_t SurfaceMaterialIndex(const clipMap_t* clipMap, const GfxSurface& surface)
    {
        const auto* materialName = surface.material && surface.material->info.name ? surface.material->info.name : nullptr;
        const auto materialBaseName = BaseName(materialName);

        if (!clipMap || !clipMap->materials)
            return 0u;

        for (auto i = 0uz; i < clipMap->numMaterials; i++)
        {
            const auto rawName = BaseName(clipMap->materials[i].material);
            if (MaterialNameMatches(rawName, materialBaseName))
                return static_cast<uint16_t>(i);
        }

        return 0u;
    }

    [[nodiscard]] uint8_t RawPrimaryLightIndex(const GfxSurface& surface)
    {
        // Sky surfaces in test.d3dbsp store 0 here; the linker normalizes them to the sun primary light at runtime.
        if (static_cast<unsigned char>(surface.lightmapIndex) == SKY_LIGHTMAP_INDEX)
            return 0u;

        return static_cast<uint8_t>(surface.primaryLightIndex);
    }

    [[nodiscard]] unsigned LightmapRawPageCount(const GfxImageLoadDef* primary, const GfxImageLoadDef* secondary)
    {
        if (!primary || !secondary || primary->resourceSize == 0u || secondary->resourceSize == 0u)
            return 1u;

        const auto primaryPages = (primary->resourceSize + LIGHTMAP_PRIMARY_RAW_PAGE_SIZE - 1uz) / LIGHTMAP_PRIMARY_RAW_PAGE_SIZE;
        const auto secondaryPages = (secondary->resourceSize + LIGHTMAP_SECONDARY_RAW_PAGE_SIZE - 1uz) / LIGHTMAP_SECONDARY_RAW_PAGE_SIZE;
        return static_cast<unsigned>(std::max(1uz, std::max(primaryPages, secondaryPages)));
    }

    [[nodiscard]] std::pair<unsigned, unsigned> LightmapAtlasGrid(const GfxImageLoadDef* primary, const GfxImageLoadDef* secondary)
    {
        const auto resourcePageCount = LightmapRawPageCount(primary, secondary);
        auto wideCount = primary && primary->dimensions[0] >= LIGHTMAP_PRIMARY_RAW_WIDTH ? primary->dimensions[0] / LIGHTMAP_PRIMARY_RAW_WIDTH : 1u;
        auto highCount =
            primary && primary->dimensions[1] >= LIGHTMAP_PRIMARY_RAW_HEIGHT ? primary->dimensions[1] / LIGHTMAP_PRIMARY_RAW_HEIGHT : resourcePageCount;

        wideCount = std::max(1u, wideCount);
        highCount = std::max(1u, highCount);

        if (wideCount * highCount != resourcePageCount)
        {
            wideCount = 1u;
            highCount = resourcePageCount;
        }

        return std::make_pair(wideCount, highCount);
    }

    [[nodiscard]] std::vector<unsigned> LinkerRawPageOrder(const unsigned pageCount)
    {
        std::vector<unsigned> result;
        result.reserve(pageCount);

        if (pageCount >= 2u)
        {
            // The linker's packing pass seeds each multi-page group with the selected pair in this order.
            // For a two-page runtime lightmap that means raw page 1 is packed into the first atlas slot and raw page 0 into the second.
            result.emplace_back(1u);
            result.emplace_back(0u);
        }
        else if (pageCount == 1u)
        {
            result.emplace_back(0u);
        }

        for (auto pageIndex = 2u; pageIndex < pageCount; pageIndex++)
            result.emplace_back(pageIndex);

        return result;
    }

    [[nodiscard]] std::vector<unsigned> InvertRawPageOrder(const std::vector<unsigned>& rawPageForPackedSlot)
    {
        std::vector<unsigned> result(rawPageForPackedSlot.size());

        for (auto packedSlot = 0uz; packedSlot < rawPageForPackedSlot.size(); packedSlot++)
        {
            const auto rawPage = rawPageForPackedSlot[packedSlot];
            if (rawPage < result.size())
                result[rawPage] = static_cast<unsigned>(packedSlot);
        }

        return result;
    }

    void AppendZeros(std::vector<std::byte>& out, const size_t size)
    {
        out.resize(out.size() + size, std::byte{});
    }

    void PadToSize(std::vector<std::byte>& out, const size_t size)
    {
        if (out.size() < size)
            AppendZeros(out, size - out.size());
    }

    void AppendPrimaryLightmapRawPage(
        std::vector<std::byte>& out, const GfxImageLoadDef& primary, const unsigned wideCount, const unsigned highCount, const unsigned packedSlot)
    {
        const auto sourceWidth = static_cast<unsigned>(primary.dimensions[0]);
        const auto sourceHeight = static_cast<unsigned>(primary.dimensions[1]);
        const auto slotX = packedSlot % wideCount;
        const auto slotY = packedSlot / wideCount;

        if (sourceWidth < wideCount * LIGHTMAP_PRIMARY_RAW_WIDTH || sourceHeight < highCount * LIGHTMAP_PRIMARY_RAW_HEIGHT)
        {
            AppendZeros(out, LIGHTMAP_PRIMARY_RAW_PAGE_SIZE);
            return;
        }

        const auto sourceStride = static_cast<size_t>(sourceWidth);
        const auto sourceX = static_cast<size_t>(slotX) * LIGHTMAP_PRIMARY_RAW_WIDTH;
        const auto sourceY = static_cast<size_t>(slotY) * LIGHTMAP_PRIMARY_RAW_HEIGHT;

        for (auto row = 0u; row < LIGHTMAP_PRIMARY_RAW_HEIGHT; row++)
        {
            const auto sourceOffset = (sourceY + row) * sourceStride + sourceX;
            if (sourceOffset + LIGHTMAP_PRIMARY_RAW_WIDTH <= primary.resourceSize)
                AppendBytes(out, primary.data + sourceOffset, LIGHTMAP_PRIMARY_RAW_WIDTH);
            else
                AppendZeros(out, LIGHTMAP_PRIMARY_RAW_WIDTH);
        }
    }

    void AppendSecondaryLightmapRawPage(
        std::vector<std::byte>& out, const GfxImageLoadDef& secondary, const unsigned wideCount, const unsigned highCount, const unsigned packedSlot)
    {
        const auto sourceWidth = static_cast<unsigned>(secondary.dimensions[0]);
        const auto sourceHeight = static_cast<unsigned>(secondary.dimensions[1]);
        const auto slotX = packedSlot % wideCount;
        const auto slotY = packedSlot / wideCount;

        if (sourceWidth < wideCount * LIGHTMAP_SECONDARY_RAW_WIDTH || sourceHeight < highCount * LIGHTMAP_SECONDARY_RAW_HEIGHT)
        {
            AppendZeros(out, LIGHTMAP_SECONDARY_RAW_PAGE_SIZE);
            return;
        }

        const auto sourceStride = static_cast<size_t>(sourceWidth) * LIGHTMAP_SECONDARY_PIXEL_SIZE;
        const auto sourceX = static_cast<size_t>(slotX) * LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE;

        for (auto row = 0u; row < LIGHTMAP_SECONDARY_HALF_HEIGHT; row++)
        {
            const auto sourceY = static_cast<size_t>(slotY) * LIGHTMAP_SECONDARY_HALF_HEIGHT + row;
            const auto sourceOffset = sourceY * sourceStride + sourceX;
            if (sourceOffset + LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE <= secondary.resourceSize)
                AppendBytes(out, secondary.data + sourceOffset, LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE);
            else
                AppendZeros(out, LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE);
        }

        for (auto row = 0u; row < LIGHTMAP_SECONDARY_HALF_HEIGHT; row++)
        {
            const auto sourceY = static_cast<size_t>(highCount + slotY) * LIGHTMAP_SECONDARY_HALF_HEIGHT + row;
            const auto sourceOffset = sourceY * sourceStride + sourceX;
            if (sourceOffset + LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE <= secondary.resourceSize)
                AppendBytes(out, secondary.data + sourceOffset, LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE);
            else
                AppendZeros(out, LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE);
        }
    }

    [[nodiscard]] LightmapPageLayout BuildLightmapPageLayout(const GfxWorld& world)
    {
        LightmapPageLayout layout;
        layout.firstRawPage.reserve(PositiveCount(world.lightmapCount));
        layout.pageCount.reserve(PositiveCount(world.lightmapCount));
        layout.wideCount.reserve(PositiveCount(world.lightmapCount));
        layout.highCount.reserve(PositiveCount(world.lightmapCount));
        layout.rawPageForPackedSlot.reserve(PositiveCount(world.lightmapCount));
        layout.packedSlotForRawPage.reserve(PositiveCount(world.lightmapCount));
        layout.rawPageCount = 0u;

        for (auto lightmapIndex = 0uz; lightmapIndex < PositiveCount(world.lightmapCount); lightmapIndex++)
        {
            auto* primary = world.lightmaps ? LoadDefForImage(world.lightmaps[lightmapIndex].primary) : nullptr;
            auto* secondary = world.lightmaps ? LoadDefForImage(world.lightmaps[lightmapIndex].secondary) : nullptr;
            if (!primary)
                primary = LoadDefForTexture(world.lightmapPrimaryTextures, lightmapIndex);
            if (!secondary)
                secondary = LoadDefForTexture(world.lightmapSecondaryTextures, lightmapIndex);

            const auto [wideCount, highCount] = LightmapAtlasGrid(primary, secondary);
            const auto pageCount = wideCount * highCount;
            const auto rawPageForPackedSlot = LinkerRawPageOrder(pageCount);
            layout.firstRawPage.emplace_back(layout.rawPageCount);
            layout.pageCount.emplace_back(pageCount);
            layout.wideCount.emplace_back(wideCount);
            layout.highCount.emplace_back(highCount);
            layout.rawPageForPackedSlot.emplace_back(rawPageForPackedSlot);
            layout.packedSlotForRawPage.emplace_back(InvertRawPageOrder(rawPageForPackedSlot));
            layout.rawPageCount += pageCount;
        }

        return layout;
    }

    [[nodiscard]] unsigned SurfacePackedLightmapSlot(const GfxWorld& world, const GfxSurface& surface, const unsigned wideCount, const unsigned highCount)
    {
        if (wideCount * highCount <= 1u || !world.indices || !world.vd.vertices || world.vertexCount == 0u)
            return 0u;

        const auto firstIndex = surface.tris.baseIndex;
        const auto indexCount = static_cast<int>(surface.tris.triCount) * 3;
        if (firstIndex < 0 || indexCount <= 0 || firstIndex + indexCount > world.indexCount)
            return 0u;

        auto minU = std::numeric_limits<float>::max();
        auto maxU = std::numeric_limits<float>::lowest();
        auto minV = std::numeric_limits<float>::max();
        auto maxV = std::numeric_limits<float>::lowest();
        auto foundVertex = false;

        for (auto indexOffset = 0; indexOffset < indexCount; indexOffset++)
        {
            const auto vertexIndex = surface.tris.firstVertex + world.indices[firstIndex + indexOffset];
            if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                continue;

            const auto u = world.vd.vertices[vertexIndex].lmapCoord[0];
            const auto v = world.vd.vertices[vertexIndex].lmapCoord[1];
            minU = std::min(minU, u);
            maxU = std::max(maxU, u);
            minV = std::min(minV, v);
            maxV = std::max(maxV, v);
            foundVertex = true;
        }

        if (!foundVertex)
            return 0u;

        const auto midpointU = (static_cast<double>(minU) + static_cast<double>(maxU)) * 0.5;
        const auto midpointV = (static_cast<double>(minV) + static_cast<double>(maxV)) * 0.5;
        const auto slotX = std::clamp(static_cast<int>(std::floor(midpointU * wideCount)), 0, static_cast<int>(wideCount - 1u));
        const auto slotY = std::clamp(static_cast<int>(std::floor(midpointV * highCount)), 0, static_cast<int>(highCount - 1u));

        return static_cast<unsigned>(slotY) * wideCount + static_cast<unsigned>(slotX);
    }

    [[nodiscard]] std::vector<SurfaceLightmapRemap> BuildSurfaceLightmapRemaps(const GfxWorld& world, const LightmapPageLayout& layout)
    {
        std::vector<SurfaceLightmapRemap> remaps;
        remaps.reserve(PositiveCount(world.surfaceCount));

        for (auto surfaceIndex = 0uz; surfaceIndex < PositiveCount(world.surfaceCount); surfaceIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto lightmapIndex = static_cast<unsigned char>(surface.lightmapIndex);
            if (lightmapIndex == SKY_LIGHTMAP_INDEX || lightmapIndex >= layout.pageCount.size())
            {
                remaps.emplace_back(SurfaceLightmapRemap{static_cast<uint8_t>(lightmapIndex), lightmapIndex, 0u, 1u, 1u});
                continue;
            }

            const auto wideCount = layout.wideCount[lightmapIndex];
            const auto highCount = layout.highCount[lightmapIndex];
            const auto packedSlot = SurfacePackedLightmapSlot(world, surface, wideCount, highCount);
            const auto rawPage =
                packedSlot < layout.rawPageForPackedSlot[lightmapIndex].size() ? layout.rawPageForPackedSlot[lightmapIndex][packedSlot] : packedSlot;
            const auto rawLightmapIndex = layout.firstRawPage[lightmapIndex] + rawPage;
            remaps.emplace_back(SurfaceLightmapRemap{static_cast<uint8_t>(rawLightmapIndex), lightmapIndex, packedSlot, wideCount, highCount});
        }

        return remaps;
    }

    [[nodiscard]] std::vector<VertexLightmapRemap> BuildVertexLightmapRemaps(const GfxWorld& world, const std::vector<SurfaceLightmapRemap>& surfaceRemaps)
    {
        std::vector<VertexLightmapRemap> remaps(world.vertexCount, VertexLightmapRemap{0u, 1u, 1u, false});
        if (!world.indices || !world.vd.vertices || !world.dpvs.surfaces)
            return remaps;

        for (auto surfaceIndex = 0uz; surfaceIndex < PositiveCount(world.surfaceCount) && surfaceIndex < surfaceRemaps.size(); surfaceIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto lightmapIndex = static_cast<unsigned char>(surface.lightmapIndex);
            if (lightmapIndex == SKY_LIGHTMAP_INDEX)
                continue;

            const auto firstIndex = surface.tris.baseIndex;
            const auto indexCount = static_cast<int>(surface.tris.triCount) * 3;
            if (firstIndex < 0 || indexCount <= 0 || firstIndex + indexCount > world.indexCount)
                continue;

            const auto& surfaceRemap = surfaceRemaps[surfaceIndex];
            for (auto indexOffset = 0; indexOffset < indexCount; indexOffset++)
            {
                const auto vertexIndex = surface.tris.firstVertex + world.indices[firstIndex + indexOffset];
                if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                    continue;

                remaps[vertexIndex] = VertexLightmapRemap{surfaceRemap.packedSlot, surfaceRemap.wideCount, surfaceRemap.highCount, true};
            }
        }

        return remaps;
    }

    [[nodiscard]] std::vector<std::byte> BuildSurfaces(const clipMap_t* clipMap, const GfxWorld& world, const std::vector<SurfaceLightmapRemap>& lightmapRemaps)
    {
        std::vector<std::byte> out;
        out.reserve(PositiveCount(world.surfaceCount) * 24uz);

        // Surface order is part of the BSP's index space. Brush models and AABB
        // ranges point into this same list, so reordering here requires remapping
        // every dependent range as well.
        for (auto surfaceIndex = 0uz; surfaceIndex < PositiveCount(world.surfaceCount); surfaceIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto materialIndex = SurfaceMaterialIndex(clipMap, surface);
            const auto lightmapIndex =
                surfaceIndex < lightmapRemaps.size() ? lightmapRemaps[surfaceIndex].rawLightmapIndex : static_cast<uint8_t>(surface.lightmapIndex);
            const auto reflectionProbeIndex = static_cast<uint8_t>(surface.reflectionProbeIndex);
            const auto primaryLightIndex = RawPrimaryLightIndex(surface);
            const auto flags = static_cast<uint8_t>(surface.flags);
            const uint16_t padding = 0u;
            const auto vertexLayerData = surface.tris.vertexLayerData;
            const auto firstVertex = surface.tris.firstVertex;
            const auto vertexCount = surface.tris.vertexCount;
            const auto indexCount = static_cast<uint16_t>(surface.tris.triCount * 3u);
            const auto baseIndex = surface.tris.baseIndex;

            Append(out, materialIndex);
            Append(out, lightmapIndex);
            Append(out, reflectionProbeIndex);
            Append(out, primaryLightIndex);
            Append(out, flags);
            Append(out, padding);
            Append(out, vertexLayerData);
            Append(out, firstVertex);
            Append(out, vertexCount);
            Append(out, indexCount);
            Append(out, baseIndex);
        }

        return out;
    }

    void CrossProduct(const float (&left)[3], const float (&right)[3], float (&out)[3])
    {
        out[0] = left[1] * right[2] - left[2] * right[1];
        out[1] = left[2] * right[0] - left[0] * right[2];
        out[2] = left[0] * right[1] - left[1] * right[0];
    }

    [[nodiscard]] std::vector<std::byte> BuildVertices(const GfxWorld& world, const std::vector<VertexLightmapRemap>& lightmapRemaps)
    {
        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(world.vertexCount) * 68uz);

        for (auto vertexIndex = 0uz; vertexIndex < world.vertexCount; vertexIndex++)
        {
            const auto& vertex = world.vd.vertices[vertexIndex];
            float normal[3]{};
            float tangent[3]{};
            float binormal[3]{};
            float lmapCoord[2]{vertex.lmapCoord[0], vertex.lmapCoord[1]};
            Common::Vec3UnpackUnitVec(vertex.normal, normal);
            Common::Vec3UnpackUnitVec(vertex.tangent, tangent);
            CrossProduct(normal, tangent, binormal);

            for (auto& component : binormal)
                component *= vertex.binormalSign;

            if (vertexIndex < lightmapRemaps.size() && lightmapRemaps[vertexIndex].valid
                && lightmapRemaps[vertexIndex].wideCount * lightmapRemaps[vertexIndex].highCount > 1u)
            {
                const auto& remap = lightmapRemaps[vertexIndex];
                const auto slotX = remap.packedSlot % remap.wideCount;
                const auto slotY = remap.packedSlot / remap.wideCount;
                lmapCoord[0] = lmapCoord[0] * static_cast<float>(remap.wideCount) - static_cast<float>(slotX);
                lmapCoord[1] = lmapCoord[1] * static_cast<float>(remap.highCount) - static_cast<float>(slotY);
            }

            AppendBytes(out, vertex.xyz, sizeof(vertex.xyz));
            AppendBytes(out, normal, sizeof(normal));
            Append(out, vertex.color.packed);
            AppendBytes(out, vertex.texCoord, sizeof(vertex.texCoord));
            AppendBytes(out, lmapCoord, sizeof(lmapCoord));
            AppendBytes(out, tangent, sizeof(tangent));
            AppendBytes(out, binormal, sizeof(binormal));
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildIndices(const GfxWorld& world)
    {
        return RawBytes(world.indices, PositiveCount(world.indexCount) * sizeof(uint16_t));
    }

    [[nodiscard]] std::vector<std::byte> BuildLightmapImages(const GfxWorld& world, const LightmapPageLayout& layout)
    {
        if (world.lightmapCount <= 0)
            return {};

        std::vector<std::byte> out;
        for (auto lightmapIndex = 0uz; lightmapIndex < PositiveCount(world.lightmapCount); lightmapIndex++)
        {
            auto* primary = world.lightmaps ? LoadDefForImage(world.lightmaps[lightmapIndex].primary) : nullptr;
            auto* secondary = world.lightmaps ? LoadDefForImage(world.lightmaps[lightmapIndex].secondary) : nullptr;
            if (!primary)
                primary = LoadDefForTexture(world.lightmapPrimaryTextures, lightmapIndex);
            if (!secondary)
                secondary = LoadDefForTexture(world.lightmapSecondaryTextures, lightmapIndex);
            if (!primary || !secondary)
                return {};

            const auto pageCount = lightmapIndex < layout.pageCount.size() ? layout.pageCount[lightmapIndex] : 1u;
            const auto wideCount = lightmapIndex < layout.wideCount.size() ? layout.wideCount[lightmapIndex] : 1u;
            const auto highCount = lightmapIndex < layout.highCount.size() ? layout.highCount[lightmapIndex] : pageCount;
            for (auto rawPage = 0u; rawPage < pageCount; rawPage++)
            {
                // Raw d3dbsp stores fixed-height lightmap pages as secondary texture data first, then primary.
                // The linker packs raw pages into a runtime atlas and applies the matching UV scale/offset when loading surfaces.
                const auto packedSlot = lightmapIndex < layout.packedSlotForRawPage.size() && rawPage < layout.packedSlotForRawPage[lightmapIndex].size()
                                            ? layout.packedSlotForRawPage[lightmapIndex][rawPage]
                                            : rawPage;
                AppendSecondaryLightmapRawPage(out, *secondary, wideCount, highCount, packedSlot);
                AppendPrimaryLightmapRawPage(out, *primary, wideCount, highCount, packedSlot);
            }
        }

        return out;
    }

    [[nodiscard]] uint32_t PackRgba(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        return static_cast<uint32_t>(r) | (static_cast<uint32_t>(g) << 8u) | (static_cast<uint32_t>(b) << 16u) | (static_cast<uint32_t>(a) << 24u);
    }

    [[nodiscard]] uint32_t TransformReflectionProbeColor(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        constexpr double LUMA_R = 0.1140000000596046;
        constexpr double LUMA_G = 0.5870000123977661;
        constexpr double LUMA_B = 0.2989999949932098;

        const auto& correction = DEFAULT_REFLECTION_PROBE_CORRECTION;
        const auto range = correction.whiteLevel - correction.blackLevel;

        double color[3]{
            std::pow(std::max(0.0, (static_cast<double>(r) / 255.0 - correction.blackLevel) / range), correction.gamma),
            std::pow(std::max(0.0, (static_cast<double>(g) / 255.0 - correction.blackLevel) / range), correction.gamma),
            std::pow(std::max(0.0, (static_cast<double>(b) / 255.0 - correction.blackLevel) / range), correction.gamma),
        };

        const auto luma = LUMA_R * color[0] + LUMA_G * color[1] + LUMA_B * color[2];
        for (auto& channel : color)
            channel = correction.saturation * channel + (1.0 - correction.saturation) * luma;

        const auto scale = std::max({0.1, color[0], color[1], color[2]});
        return PackRgba(ClampToByte(static_cast<int>(color[0] / scale * 255.0)),
                        ClampToByte(static_cast<int>(color[1] / scale * 255.0)),
                        ClampToByte(static_cast<int>(color[2] / scale * 255.0)),
                        ClampToByte(static_cast<int>(255.0 * (scale * 0.25))));
    }

    [[nodiscard]] uint32_t EstimateReflectionProbeSourceColor(const uint32_t targetColor, const double scale)
    {
        constexpr double LUMA_R = 0.1140000000596046;
        constexpr double LUMA_G = 0.5870000123977661;
        constexpr double LUMA_B = 0.2989999949932098;

        const auto& correction = DEFAULT_REFLECTION_PROBE_CORRECTION;
        const auto r = static_cast<double>(targetColor & 0xFFu);
        const auto g = static_cast<double>((targetColor >> 8u) & 0xFFu);
        const auto b = static_cast<double>((targetColor >> 16u) & 0xFFu);

        const double corrected[3]{
            ((r + 0.5) / 255.0) * scale,
            ((g + 0.5) / 255.0) * scale,
            ((b + 0.5) / 255.0) * scale,
        };

        const auto luma = LUMA_R * corrected[0] + LUMA_G * corrected[1] + LUMA_B * corrected[2];
        const auto range = correction.whiteLevel - correction.blackLevel;

        uint8_t source[3]{};
        for (auto i = 0uz; i < 3uz; i++)
        {
            const auto unsaturated = std::max(0.0, (corrected[i] - (1.0 - correction.saturation) * luma) / correction.saturation);
            const auto linear = std::pow(unsaturated, 1.0 / correction.gamma) * range + correction.blackLevel;
            source[i] = ClampToByte(static_cast<int>(std::round(linear * 255.0)));
        }

        return PackRgba(source[0], source[1], source[2], 0u);
    }

    [[nodiscard]] uint32_t InvertReflectionProbeColor(const uint32_t targetColor, std::unordered_map<uint32_t, uint32_t>& cache)
    {
        const auto cached = cache.find(targetColor);
        if (cached != cache.end())
            return cached->second;

        const auto alpha = static_cast<double>((targetColor >> 24u) & 0xFFu);
        const auto minScale = alpha / 63.75;
        const auto maxScale = (alpha + 1.0) / 63.75;
        const auto estimated = EstimateReflectionProbeSourceColor(targetColor, (minScale + maxScale) * 0.5);

        const int base[3]{
            static_cast<int>(estimated & 0xFFu),
            static_cast<int>((estimated >> 8u) & 0xFFu),
            static_cast<int>((estimated >> 16u) & 0xFFu),
        };

        constexpr auto SEARCH_RADIUS = 5;
        for (auto r = std::max(0, base[0] - SEARCH_RADIUS); r <= std::min(255, base[0] + SEARCH_RADIUS); r++)
        {
            for (auto g = std::max(0, base[1] - SEARCH_RADIUS); g <= std::min(255, base[1] + SEARCH_RADIUS); g++)
            {
                for (auto b = std::max(0, base[2] - SEARCH_RADIUS); b <= std::min(255, base[2] + SEARCH_RADIUS); b++)
                {
                    if (TransformReflectionProbeColor(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)) == targetColor)
                    {
                        const auto sourceColor = PackRgba(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), 0u);
                        cache.emplace(targetColor, sourceColor);
                        return sourceColor;
                    }
                }
            }
        }

        // Some transformed colors may not have an exact source-byte preimage under the default correction.
        cache.emplace(targetColor, estimated);
        return estimated;
    }

    [[nodiscard]] size_t ReflectionProbeMipSize(const size_t mipLevel)
    {
        const auto size = REFLECTION_PROBE_SIZE >> mipLevel;
        return size * size * REFLECTION_PROBE_PIXEL_SIZE;
    }

    [[nodiscard]] size_t ReflectionProbeTextureOffset(const size_t face, const size_t mipLevel)
    {
        auto offset = 0uz;
        for (auto currentMip = 0uz; currentMip < mipLevel; currentMip++)
            offset += ReflectionProbeMipSize(currentMip) * 6uz;

        return offset + ReflectionProbeMipSize(mipLevel) * face;
    }

    void AppendInvertedReflectionProbePixels(std::vector<std::byte>& out, const char* imageData, std::unordered_map<uint32_t, uint32_t>& colorCache)
    {
        const auto* bytes = reinterpret_cast<const uint8_t*>(imageData);
        const auto appendFaceMip = [&out, bytes, &colorCache](const size_t face, const size_t mipLevel)
        {
            const auto sourceOffset = ReflectionProbeTextureOffset(face, mipLevel);
            const auto pixelCount = ReflectionProbeMipSize(mipLevel) / REFLECTION_PROBE_PIXEL_SIZE;
            for (auto pixelIndex = 0uz; pixelIndex < pixelCount; pixelIndex++)
            {
                const auto pixelOffset = sourceOffset + pixelIndex * REFLECTION_PROBE_PIXEL_SIZE;
                const auto targetColor = PackRgba(bytes[pixelOffset], bytes[pixelOffset + 1uz], bytes[pixelOffset + 2uz], bytes[pixelOffset + 3uz]);
                const auto sourceColor = InvertReflectionProbeColor(targetColor, colorCache);
                Append(out, sourceColor);
            }
        };

        for (auto face = 0uz; face < 6uz; face++)
            appendFaceMip(face, 0uz);

        for (auto face = 0uz; face < 6uz; face++)
        {
            for (auto mipLevel = 1uz; mipLevel < REFLECTION_PROBE_MIP_COUNT; mipLevel++)
                appendFaceMip(face, mipLevel);
        }
    }

    [[nodiscard]] std::vector<std::byte> BuildReflectionProbeRecords(const GfxWorld& world)
    {
        if (world.reflectionProbeCount <= 1u || !world.reflectionProbes)
            return {};

        std::vector<std::byte> out;
        std::unordered_map<uint32_t, uint32_t> colorCache;
        // The linker discards the source color-correction name after baking the probe image.
        // Emit a canonical empty/default record and invert pixels so relinking applies the same default transform.
        const std::array<std::byte, REFLECTION_PROBE_NAME_SIZE> correctionName{};

        for (auto probeIndex = 1uz; probeIndex < world.reflectionProbeCount; probeIndex++)
        {
            const auto& probe = world.reflectionProbes[probeIndex];
            auto* loadDef = LoadDefForImage(probe.reflectionImage);
            if (!loadDef)
                loadDef = LoadDefForTexture(world.reflectionProbeTextures, probeIndex);

            if (!loadDef || loadDef->resourceSize != REFLECTION_PROBE_RAW_DATA_SIZE)
                return {};

            AppendBytes(out, probe.origin, sizeof(probe.origin));
            AppendBytes(out, correctionName.data(), correctionName.size());
            AppendInvertedReflectionProbePixels(out, loadDef->data, colorCache);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildCellHeader(const GfxWorld& world)
    {
        constexpr auto CELL_HEADER_SIZE = 112uz;
        constexpr auto REFLECTION_PROBE_LIST_OFFSET = 44uz;
        constexpr auto REFLECTION_PROBE_LIST_CAPACITY = CELL_HEADER_SIZE - REFLECTION_PROBE_LIST_OFFSET - 1uz;

        std::vector<std::byte> out;
        out.reserve(CELL_HEADER_SIZE);

        if (world.dpvsPlanes.cellCount <= 0 || !world.cells)
        {
            out.resize(CELL_HEADER_SIZE, std::byte{});
            return out;
        }

        const auto& cell = world.cells[0];
        AppendBytes(out, cell.mins, sizeof(cell.mins));
        AppendBytes(out, cell.maxs, sizeof(cell.maxs));

        // The fixed-size cell header stores the cell's reflection probe list at byte 44.
        PadToSize(out, REFLECTION_PROBE_LIST_OFFSET);
        out.emplace_back(static_cast<std::byte>(cell.reflectionProbeCount));
        for (auto i = 0uz; i < static_cast<unsigned char>(cell.reflectionProbeCount) && i < REFLECTION_PROBE_LIST_CAPACITY; i++)
            out.emplace_back(static_cast<std::byte>(cell.reflectionProbes[i]));

        PadToSize(out, CELL_HEADER_SIZE);
        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildBrushModels(const clipMap_t* clipMap, const GfxWorld& world)
    {
        std::vector<std::byte> out;
        out.reserve(PositiveCount(world.modelCount) * 48uz);

        for (auto modelIndex = 0uz; modelIndex < PositiveCount(world.modelCount); modelIndex++)
        {
            const auto& model = world.models[modelIndex];
            const auto startSurfIndex = static_cast<uint16_t>(model.startSurfIndex);
            const auto surfaceCount = model.surfaceCount;
            const auto surfaceCountNoDecal = model.surfaceCountNoDecal;
            const auto firstTriSoup = surfaceCount > 0u ? startSurfIndex : std::numeric_limits<uint16_t>::max();
            const auto firstTriSoupNoDecal = surfaceCountNoDecal > 0u ? startSurfIndex : std::numeric_limits<uint16_t>::max();
            auto firstCollAabbIndex = 0u;
            auto collAabbCount = 0u;
            auto firstBrush = 0u;
            auto brushCount = modelIndex == 0uz && clipMap ? static_cast<uint32_t>(clipMap->numBrushes) : 0u;

            if (clipMap && modelIndex > 0uz && modelIndex < clipMap->numSubModels)
            {
                const auto& cmodel = clipMap->cmodels[modelIndex];
                firstCollAabbIndex = static_cast<uint16_t>(cmodel.leaf.firstCollAabbIndex);
                collAabbCount = static_cast<uint16_t>(cmodel.leaf.collAabbCount);

                // linker_pc derives submodel collision leaves from these raw
                // brush ranges. Runtime clipMap only keeps the partitioned
                // leafbrush tree, so flatten it back to a contiguous range.
                std::vector<LeafBrush> modelBrushes;
                if (cmodel.leaf.leafBrushNode > 0 && CollectLeafBrushes_r(*clipMap, static_cast<size_t>(cmodel.leaf.leafBrushNode), modelBrushes)
                    && !modelBrushes.empty())
                {
                    const auto [minBrush, maxBrush] = std::minmax_element(modelBrushes.begin(), modelBrushes.end());
                    firstBrush = *minBrush;
                    brushCount = static_cast<uint32_t>(*maxBrush - *minBrush + 1u);
                }
            }

            AppendBytes(out, model.bounds[0], sizeof(model.bounds[0]));
            AppendBytes(out, model.bounds[1], sizeof(model.bounds[1]));
            // Raw v22 stores firstTriSoup[2] and triSoupCount[2]. R_LoadSubmodels
            // maps an empty tri-soup range to startSurfIndex 0xffff.
            Append(out, firstTriSoupNoDecal);
            Append(out, firstTriSoup);
            Append(out, surfaceCountNoDecal);
            Append(out, surfaceCount);
            // The render fields above are 16-bit, but the following collision
            // and brush ranges are 32-bit in the raw model record.
            Append(out, firstCollAabbIndex);
            Append(out, collAabbCount);
            Append(out, firstBrush);
            Append(out, brushCount);
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildAabbSurfaceRanges(const GfxWorld& world)
    {
        std::vector<std::byte> out;

        if (world.dpvs.staticSurfaceCount != world.dpvs.staticSurfaceCountNoDecal)
        {
            const uint32_t startSurfIndex = 0u;
            const auto surfaceCount =
                world.modelCount > 0 && world.models ? static_cast<uint32_t>(world.models[0].surfaceCount) : static_cast<uint32_t>(world.surfaceCount);
            const uint32_t childCount = 0u;
            Append(out, startSurfIndex);
            Append(out, surfaceCount);
            Append(out, childCount);

            // The loader does not yet reconstruct the original per-cell AABB
            // hierarchy. For decal-split maps, emit one root leaf covering
            // model0 so Radiant's no-decal compaction sees the full surface set.
            // Runtime cell tree counts include static-model tree fixups and are
            // not stable between linker_pc-origin and OAT-origin fastfiles, so
            // do not preserve their zero-padding footprint in the raw lump.

            return out;
        }

        if (world.dpvsPlanes.cellCount <= 0 || !world.cells)
            return out;

        for (auto cellIndex = 0; cellIndex < world.dpvsPlanes.cellCount; cellIndex++)
        {
            const auto& cell = world.cells[cellIndex];
            for (auto treeIndex = 0; treeIndex < cell.aabbTreeCount; treeIndex++)
            {
                const auto& tree = cell.aabbTree[treeIndex];
                const auto startSurfIndex = static_cast<uint32_t>(tree.startSurfIndex);
                const auto surfaceCount = static_cast<uint32_t>(tree.surfaceCount);
                const auto childCount = static_cast<uint32_t>(tree.childCount);

                Append(out, startSurfIndex);
                Append(out, surfaceCount);
                Append(out, childCount);
            }
        }

        return out;
    }

    [[nodiscard]] bool CanReuseAabbSurfaceRangesForLegacyLump(const GfxWorld& world)
    {
        // The old render path only matches the new AABB surface ranges when there is no decal split.
        // Maps with decal-split surfaces need the original legacy ranges, which are not recovered yet.
        if (world.dpvs.staticSurfaceCount != world.dpvs.staticSurfaceCountNoDecal)
            return false;

        return true;
    }

    [[nodiscard]] std::string FormatFloat(const float value)
    {
        auto result = std::format("{:.9g}", value);
        if (result == "-0")
            return "0";

        return result;
    }

    [[nodiscard]] float StaticModelScale(const cStaticModel_s& staticModel)
    {
        const auto invScale =
            std::sqrt(staticModel.invScaledAxis[0][0] * staticModel.invScaledAxis[0][0] + staticModel.invScaledAxis[0][1] * staticModel.invScaledAxis[0][1]
                      + staticModel.invScaledAxis[0][2] * staticModel.invScaledAxis[0][2]);
        if (invScale <= std::numeric_limits<float>::epsilon())
            return 1.0f;

        return 1.0f / invScale;
    }

    void StaticModelAxis(const cStaticModel_s& staticModel, float (&axis)[3][3])
    {
        const auto scale = StaticModelScale(staticModel);
        for (auto row = 0uz; row < 3uz; row++)
        {
            for (auto column = 0uz; column < 3uz; column++)
                axis[row][column] = staticModel.invScaledAxis[column][row] * scale;
        }
    }

    [[nodiscard]] std::array<float, 3> StaticModelAngles(const cStaticModel_s& staticModel)
    {
        float axis[3][3]{};
        StaticModelAxis(staticModel, axis);

        const auto forwardLength = std::sqrt(axis[0][0] * axis[0][0] + axis[0][1] * axis[0][1]);
        return {static_cast<float>(std::atan2(-axis[0][2], forwardLength) * 180.0 / PI),
                static_cast<float>(std::atan2(axis[0][1], axis[0][0]) * 180.0 / PI),
                static_cast<float>(std::atan2(axis[1][2], axis[2][2]) * 180.0 / PI)};
    }

    [[nodiscard]] bool AlmostEqual(const float a, const float b)
    {
        return std::abs(a - b) <= 0.001f;
    }

    [[nodiscard]] bool StaticModelOriginMatches(const cStaticModel_s& staticModel, const GfxStaticModelDrawInst& drawInst)
    {
        return AlmostEqual(staticModel.origin[0], drawInst.placement.origin[0]) && AlmostEqual(staticModel.origin[1], drawInst.placement.origin[1])
               && AlmostEqual(staticModel.origin[2], drawInst.placement.origin[2]);
    }

    [[nodiscard]] bool StaticModelNameMatches(const cStaticModel_s& staticModel, const GfxStaticModelDrawInst& drawInst)
    {
        const auto* staticModelName = NameOf(staticModel.xmodel);
        const auto* drawModelName = NameOf(drawInst.model);
        return staticModelName && drawModelName && std::strcmp(staticModelName, drawModelName) == 0;
    }

    [[nodiscard]] size_t FindStaticModelDrawIndex(const GfxWorld* world, const cStaticModel_s& staticModel, const std::vector<bool>& usedDrawInsts)
    {
        if (!world || !world->dpvs.smodelDrawInsts)
            return INVALID_STATIC_MODEL_INDEX;

        for (auto drawIndex = 0uz; drawIndex < PositiveCount(world->dpvs.smodelCount); drawIndex++)
        {
            if (drawIndex < usedDrawInsts.size() && usedDrawInsts[drawIndex])
                continue;

            const auto& drawInst = world->dpvs.smodelDrawInsts[drawIndex];
            if (StaticModelNameMatches(staticModel, drawInst) && StaticModelOriginMatches(staticModel, drawInst))
                return drawIndex;
        }

        return INVALID_STATIC_MODEL_INDEX;
    }

    [[nodiscard]] std::string StaticModelGroundLighting(const GfxStaticModelInst& inst, const GfxStaticModelDrawInst& drawInst)
    {
        // The linker parses gndLt with "%02x%02x%02x%02x%02x" and stores the
        // first four fields into groundLighting as B, G, R, alpha. For v22 the
        // fifth field is stored directly as smodelDrawInst->primaryLightIndex.
        const auto& color = inst.groundLighting.array;
        return std::format("{:02x}{:02x}{:02x}{:02x}{:02x}",
                           static_cast<unsigned>(Byte(color[2])),
                           static_cast<unsigned>(Byte(color[1])),
                           static_cast<unsigned>(Byte(color[0])),
                           static_cast<unsigned>(Byte(color[3])),
                           static_cast<unsigned>(Byte(drawInst.primaryLightIndex)));
    }

    void AppendStaticModelGroundLighting(std::string& out, const GfxWorld* world, const size_t staticModelIndex)
    {
        if (!world || staticModelIndex == INVALID_STATIC_MODEL_INDEX || !world->dpvs.smodelInsts || !world->dpvs.smodelDrawInsts
            || staticModelIndex >= PositiveCount(world->dpvs.smodelCount))
            return;

        const auto& inst = world->dpvs.smodelInsts[staticModelIndex];
        const auto& drawInst = world->dpvs.smodelDrawInsts[staticModelIndex];
        // The stock caller only keeps parsed gndLt when the packed ground
        // lighting color is non-zero; otherwise it recomputes model lighting.
        if (inst.groundLighting.packed == 0u)
            return;

        out += std::format("\"gndLt\" \"{}\"\n", StaticModelGroundLighting(inst, drawInst));
    }

    void AppendStaticModelSpawnFlags(std::string& out, const GfxWorld* world, const size_t staticModelIndex)
    {
        if (!world || staticModelIndex == INVALID_STATIC_MODEL_INDEX || !world->dpvs.smodelDrawInsts
            || staticModelIndex >= PositiveCount(world->dpvs.smodelCount))
            return;

        // The linker only reads misc_model spawnflags bit 2 for this field:
        // smodelDrawInst->flags = (spawnflags & 2) != 0.
        if (Byte(world->dpvs.smodelDrawInsts[staticModelIndex].flags) != 0u)
            out += "\"spawnflags\" \"2\"\n";
    }

    void AppendStaticModelEntity(std::string& out, const cStaticModel_s& staticModel, const GfxWorld* world, const size_t staticModelIndex)
    {
        const auto* modelName = NameOf(staticModel.xmodel);
        if (!modelName || !*modelName)
            return;

        const auto angles = StaticModelAngles(staticModel);
        out += "{\n";
        out += std::format("\"modelscale\" \"{}\"\n", FormatFloat(StaticModelScale(staticModel)));
        out += std::format(
            "\"origin\" \"{} {} {}\"\n", FormatFloat(staticModel.origin[0]), FormatFloat(staticModel.origin[1]), FormatFloat(staticModel.origin[2]));
        out += std::format("\"angles\" \"{} {} {}\"\n", FormatFloat(angles[0]), FormatFloat(angles[1]), FormatFloat(angles[2]));
        AppendStaticModelSpawnFlags(out, world, staticModelIndex);
        AppendStaticModelGroundLighting(out, world, staticModelIndex);
        out += std::format("\"model\" \"{}\"\n", modelName);
        out += "\"classname\" \"misc_model\"\n";
        out += "}\n";
    }

    void AppendStaticModelEntities(std::string& out, const clipMap_t* clipMap, const GfxWorld* world)
    {
        if (!clipMap || !clipMap->staticModelList)
            return;

        if (!out.empty() && out.back() != '\n')
            out += '\n';

        std::vector<bool> usedDrawInsts(world ? PositiveCount(world->dpvs.smodelCount) : 0uz);

        // Static models originate as misc_model entity text in raw d3dbsp, but
        // linking moves placement/collision data into clipMap.staticModelList
        // and lighting/render data into GfxWorld DPVS arrays. The order can
        // differ, so match the DPVS entry by model name and origin.
        for (auto staticModelIndex = 0uz; staticModelIndex < clipMap->numStaticModels; staticModelIndex++)
        {
            const auto drawIndex = FindStaticModelDrawIndex(world, clipMap->staticModelList[staticModelIndex], usedDrawInsts);
            if (drawIndex != INVALID_STATIC_MODEL_INDEX && drawIndex < usedDrawInsts.size())
                usedDrawInsts[drawIndex] = true;

            AppendStaticModelEntity(out, clipMap->staticModelList[staticModelIndex], world, drawIndex);
        }
    }

    [[nodiscard]] std::vector<std::byte> BuildEntities(const MapEnts& mapEnts, const clipMap_t* clipMap, const GfxWorld* world)
    {
        auto entityCharCount = PositiveCount(mapEnts.numEntityChars);
        if (entityCharCount > 0uz && mapEnts.entityString && mapEnts.entityString[entityCharCount - 1uz] == '\0')
            entityCharCount--;

        std::string entities;
        if (mapEnts.entityString && entityCharCount > 0uz)
            entities.assign(mapEnts.entityString, entityCharCount);

        AppendStaticModelEntities(entities, clipMap, world);
        if (!entities.empty() && entities.back() != '\n')
            entities += '\n';

        std::vector<std::byte> out;
        AppendBytes(out, entities.data(), entities.size());
        out.emplace_back(std::byte{0});
        return out;
    }

    void AppendLiteralRle(std::vector<std::byte>& out, const char* data, const size_t size)
    {
        auto offset = 0uz;
        while (offset < size)
        {
            const auto chunkSize = std::min(127uz, size - offset);
            const auto marker = static_cast<uint8_t>(~chunkSize);
            Append(out, marker);
            AppendBytes(out, data + offset, chunkSize);
            offset += chunkSize;
        }
    }

    [[nodiscard]] std::vector<std::byte> BuildGameWorldSpPath(const GameWorldSp& gameWorld)
    {
        std::vector<std::byte> out;
        const uint32_t version = 8u;
        const auto nodeCount = static_cast<uint16_t>(std::min(gameWorld.path.nodeCount, static_cast<unsigned>(std::numeric_limits<uint16_t>::max())));

        Append(out, version);
        Append(out, nodeCount);

        for (auto nodeIndex = 0uz; nodeIndex < nodeCount; nodeIndex++)
        {
            const auto& node = gameWorld.path.nodes[nodeIndex];
            const auto linkCount = node.constant.totalLinkCount;
            Append(out, linkCount);

            for (auto linkIndex = 0uz; linkIndex < linkCount; linkIndex++)
            {
                const auto& link = node.constant.Links[linkIndex];
                Append(out, link.nodeNum);
                Append(out, link.fDist);
            }
        }

        AppendLiteralRle(out, gameWorld.path.pathVis, PositiveCount(gameWorld.path.visBytes));
        return out;
    }

    void AppendPrimaryLight(std::vector<std::byte>& out, const ComPrimaryLight& light)
    {
        constexpr auto TYPE_OFFSET = 0uz;
        constexpr auto CAN_USE_SHADOW_MAP_OFFSET = 1uz;
        constexpr auto UNUSED_OFFSET = 2uz;
        constexpr auto COLOR_OFFSET = 4uz;
        constexpr auto DIR_OFFSET = 16uz;
        constexpr auto ORIGIN_OFFSET = 28uz;
        constexpr auto RADIUS_OFFSET = 40uz;
        constexpr auto COS_HALF_FOV_OUTER_OFFSET = 44uz;
        constexpr auto COS_HALF_FOV_INNER_OFFSET = 48uz;
        constexpr auto EXPONENT_OFFSET = 52uz;
        constexpr auto ROTATION_LIMIT_OFFSET = 56uz;
        constexpr auto TRANSLATION_LIMIT_OFFSET = 60uz;
        constexpr auto DEF_NAME_OFFSET = 64uz;
        constexpr auto DEF_NAME_SIZE = 64uz;

        const auto baseOffset = out.size();
        out.resize(out.size() + IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE, std::byte{});

        out[baseOffset + TYPE_OFFSET] = static_cast<std::byte>(light.type);
        out[baseOffset + CAN_USE_SHADOW_MAP_OFFSET] = static_cast<std::byte>(light.canUseShadowMap);
        out[baseOffset + UNUSED_OFFSET] = static_cast<std::byte>(light.unused);
        out[baseOffset + UNUSED_OFFSET + 1uz] = std::byte{};

        // The v22 BSP stores DiskPrimaryLight, not the runtime ComPrimaryLight
        // layout. cosHalfFovExpanded is derived by the linker from outer FOV and
        // rotationLimit, while exponent is stored as a 32-bit disk field.
        WriteBytesAt(out, baseOffset + COLOR_OFFSET, light.color, sizeof(light.color));
        WriteBytesAt(out, baseOffset + DIR_OFFSET, light.dir, sizeof(light.dir));
        WriteBytesAt(out, baseOffset + ORIGIN_OFFSET, light.origin, sizeof(light.origin));
        WriteAt(out, baseOffset + RADIUS_OFFSET, light.radius);
        WriteAt(out, baseOffset + COS_HALF_FOV_OUTER_OFFSET, light.cosHalfFovOuter);
        WriteAt(out, baseOffset + COS_HALF_FOV_INNER_OFFSET, light.cosHalfFovInner);
        const auto exponent = static_cast<int32_t>(static_cast<unsigned char>(light.exponent));
        WriteAt(out, baseOffset + EXPONENT_OFFSET, exponent);
        WriteAt(out, baseOffset + ROTATION_LIMIT_OFFSET, light.rotationLimit);
        WriteAt(out, baseOffset + TRANSLATION_LIMIT_OFFSET, light.translationLimit);
        if (light.defName)
        {
            const auto defNameLength = std::min(std::strlen(light.defName), DEF_NAME_SIZE - 1uz);
            WriteBytesAt(out, baseOffset + DEF_NAME_OFFSET, light.defName, defNameLength);
        }
    }

    [[nodiscard]] std::vector<std::byte> BuildPrimaryLights(const ComWorld& comWorld)
    {
        if (comWorld.primaryLightCount <= 0u || !comWorld.primaryLights)
            return {};

        std::vector<std::byte> out;
        out.reserve(static_cast<size_t>(comWorld.primaryLightCount) * IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE);
        for (auto lightIndex = 0uz; lightIndex < comWorld.primaryLightCount; lightIndex++)
            AppendPrimaryLight(out, comWorld.primaryLights[lightIndex]);
        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLightRegionCounts(const GfxWorld& world, const unsigned primaryLightCount)
    {
        std::vector<std::byte> out;
        out.reserve(primaryLightCount);

        for (auto lightIndex = 0uz; lightIndex < primaryLightCount; lightIndex++)
        {
            const auto hullCount =
                world.lightRegion && lightIndex < world.primaryLightCount && world.lightRegion[lightIndex].hulls ? world.lightRegion[lightIndex].hullCount : 0u;
            out.emplace_back(static_cast<std::byte>(std::min(hullCount, 0xFFu)));
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLightRegionHulls(const GfxWorld& world, const unsigned primaryLightCount)
    {
        std::vector<std::byte> out;
        if (!world.lightRegion)
            return out;

        for (auto lightIndex = 0uz; lightIndex < primaryLightCount && lightIndex < world.primaryLightCount; lightIndex++)
        {
            const auto& region = world.lightRegion[lightIndex];
            const auto hullCount = region.hulls ? std::min(region.hullCount, 0xFFu) : 0u;
            for (auto hullIndex = 0uz; hullIndex < hullCount; hullIndex++)
            {
                const auto& hull = region.hulls[hullIndex];
                const auto axisCount = hull.axis ? hull.axisCount : 0u;
                AppendBytes(out, hull.kdopMidPoint, sizeof(hull.kdopMidPoint));
                AppendBytes(out, hull.kdopHalfSize, sizeof(hull.kdopHalfSize));
                Append(out, axisCount);
            }
        }

        return out;
    }

    [[nodiscard]] std::vector<std::byte> BuildLightRegionAxes(const GfxWorld& world, const unsigned primaryLightCount)
    {
        std::vector<std::byte> out;
        if (!world.lightRegion)
            return out;

        for (auto lightIndex = 0uz; lightIndex < primaryLightCount && lightIndex < world.primaryLightCount; lightIndex++)
        {
            const auto& region = world.lightRegion[lightIndex];
            const auto hullCount = region.hulls ? std::min(region.hullCount, 0xFFu) : 0u;
            for (auto hullIndex = 0uz; hullIndex < hullCount; hullIndex++)
            {
                const auto& hull = region.hulls[hullIndex];
                if (hull.axis)
                    AppendBytes(out, hull.axis, static_cast<size_t>(hull.axisCount) * sizeof(GfxLightRegionAxis));
            }
        }

        return out;
    }

    void AddLump(std::vector<BspLump>& lumps, const IW3::d3dbsp::LumpType id, std::vector<std::byte>&& data)
    {
        if (!data.empty())
            lumps.emplace_back(id, std::move(data));
    }

    [[nodiscard]] size_t LumpOrderIndex(const IW3::d3dbsp::LumpType id)
    {
        for (auto i = 0uz; i < IW3::d3dbsp::LUMP_WRITE_ORDER.size(); i++)
        {
            if (IW3::d3dbsp::LUMP_WRITE_ORDER[i] == id)
                return i;
        }

        return IW3::d3dbsp::LUMP_WRITE_ORDER.size();
    }

    void SortLumps(std::vector<BspLump>& lumps)
    {
        std::stable_sort(lumps.begin(),
                         lumps.end(),
                         [](const BspLump& left, const BspLump& right)
                         {
                             return LumpOrderIndex(left.id) < LumpOrderIndex(right.id);
                         });
    }

    void WriteBsp(std::ostream& stream, std::vector<BspLump> lumps)
    {
        SortLumps(lumps);

        const auto lumpCount = static_cast<uint32_t>(lumps.size());
        stream::Write(stream, IW3::d3dbsp::BSP_MAGIC.data(), IW3::d3dbsp::BSP_MAGIC.size());
        stream::WriteValue(stream, IW3::d3dbsp::BSP_VERSION);
        stream::WriteValue(stream, lumpCount);

        for (const auto& lump : lumps)
        {
            const auto id = std::to_underlying(lump.id);
            const auto size = static_cast<uint32_t>(lump.data.size());
            stream::WriteValue(stream, id);
            stream::WriteValue(stream, size);
        }

        const std::byte padding[3]{};
        for (const auto& lump : lumps)
        {
            stream::Write(stream, lump.data.data(), lump.data.size());

            const auto paddingSize = (4uz - lump.data.size() % 4uz) % 4uz;
            if (paddingSize > 0)
                stream::Write(stream, padding, paddingSize);
        }
    }
} // namespace

namespace map_d3dbsp
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<GfxWorld>& asset)
    {
        const auto* world = asset.Asset();
        const auto* clipMapInfo = context.m_zone.m_pools.GetAsset<AssetClipMapPvs>(asset.m_name);
        const auto* comWorldInfo = context.m_zone.m_pools.GetAsset<AssetComWorld>(asset.m_name);
        const auto* mapEntsInfo = context.m_zone.m_pools.GetAsset<AssetMapEnts>(asset.m_name);
        const auto* gameWorldSpInfo = context.m_zone.m_pools.GetAsset<AssetGameWorldSp>(asset.m_name);
        const auto* clipMap = clipMapInfo ? clipMapInfo->Asset() : nullptr;
        const auto* comWorld = comWorldInfo ? comWorldInfo->Asset() : nullptr;
        const auto* mapEnts = mapEntsInfo ? mapEntsInfo->Asset() : nullptr;
        const auto* gameWorldSp = gameWorldSpInfo ? gameWorldSpInfo->Asset() : nullptr;

        // A raw d3dbsp is reconstructed from several loaded map assets. GfxWorld
        // alone does not contain enough data.
        assert(world);
        assert(clipMap);
        assert(comWorld);
        assert(mapEnts);
        if (!world || !clipMap || !comWorld || !mapEnts)
            return;

        // Lump 43 is ComWorld primary-light data. Light-region lumps are sized
        // by GfxWorld::primaryLightCount, which can be smaller than the ComWorld
        // record count in stock BSPs.
        const auto worldPrimaryLightCount = world->primaryLightCount;
        const auto lightmapPageLayout = BuildLightmapPageLayout(*world);
        const auto surfaceLightmapRemaps = BuildSurfaceLightmapRemaps(*world, lightmapPageLayout);
        const auto vertexLightmapRemaps = BuildVertexLightmapRemaps(*world, surfaceLightmapRemaps);

        std::vector<BspLump> lumps;

        AddLump(lumps, LUMP_MATERIALS, BuildMaterials(*clipMap));
        AddLump(lumps, LUMP_PLANES, BuildPlanes(*clipMap));
        AddLump(lumps, LUMP_BRUSHSIDES, BuildBrushSideData(*clipMap));
        AddLump(lumps, LUMP_BRUSHSIDE_EDGE_COUNTS, BuildBrushEdgeCounts(*clipMap));
        AddLump(lumps, LUMP_BRUSHEDGES, BuildBrushEdges(*clipMap));
        AddLump(lumps, LUMP_BRUSHES, BuildBrushHeaders(*clipMap));
        AddLump(lumps, LUMP_NODES, BuildClipNodes(*clipMap, *world));
        AddLump(lumps, LUMP_LEAFS, BuildClipLeafs(*clipMap));
        AddLump(lumps, LUMP_LEAFBRUSHES, BuildLeafBrushes(*clipMap));
        AddLump(lumps, LUMP_COLLISIONVERTS, BuildCollisionVerts(*clipMap));
        AddLump(lumps, LUMP_COLLISIONTRIS, BuildCollisionTriIndices(*clipMap));
        AddLump(lumps, LUMP_COLLISION_EDGE_WALKABLE, BuildCollisionTriEdgeIsWalkable(*clipMap));
        AddLump(lumps, LUMP_COLLISIONBORDERS, BuildCollisionBorders(*clipMap));
        AddLump(lumps, LUMP_COLLISIONPARTITIONS, BuildCollisionPartitions(*clipMap));
        AddLump(lumps, LUMP_COLLISIONAABBS, BuildCollisionAabbTrees(*clipMap));

        AddLump(lumps, LUMP_LIGHTGRID_HEADER, BuildLightGridHeader(*world));
        AddLump(lumps, LUMP_LIGHTGRID_ROWS, BuildLightGridRawRows(*world));
        AddLump(lumps, LUMP_LIGHTMAPS, BuildLightmapImages(*world, lightmapPageLayout));
        AddLump(lumps, LUMP_LIGHTGRID_ENTRIES, BuildLightGridEntries(*world));
        AddLump(lumps, LUMP_LIGHTGRID_COLORS, BuildLightGridColors(*world));
        AddLump(lumps, LUMP_LAYERED_TRI_SOUPS, BuildSurfaces(clipMap, *world, surfaceLightmapRemaps));
        AddLump(lumps, LUMP_LAYERED_VERTS, BuildVertices(*world, vertexLightmapRemaps));
        AddLump(lumps, LUMP_LAYERED_INDICES, BuildIndices(*world));
        if (CanReuseAabbSurfaceRangesForLegacyLump(*world))
            AddLump(lumps, LUMP_LAYERED_AABBTREES, BuildAabbSurfaceRanges(*world));
        AddLump(lumps, LUMP_CELLS, BuildCellHeader(*world));
        AddLump(lumps, LUMP_MODELS, BuildBrushModels(clipMap, *world));
        AddLump(lumps, LUMP_SIMPLE_TRI_SOUPS, BuildSurfaces(clipMap, *world, surfaceLightmapRemaps));
        AddLump(lumps, LUMP_SIMPLE_VERTS, BuildVertices(*world, vertexLightmapRemaps));
        AddLump(lumps, LUMP_SIMPLE_INDICES, BuildIndices(*world));
        AddLump(lumps, LUMP_SIMPLE_AABBTREES, BuildAabbSurfaceRanges(*world));
        AddLump(lumps, LUMP_REFLECTION_PROBES, BuildReflectionProbeRecords(*world));

        AddLump(lumps, LUMP_ENTITIES, BuildEntities(*mapEnts, clipMap, world));
        AddLump(lumps, LUMP_PRIMARY_LIGHTS, BuildPrimaryLights(*comWorld));

        AddLump(lumps, LUMP_LIGHT_REGION_COUNTS, BuildLightRegionCounts(*world, worldPrimaryLightCount));
        AddLump(lumps, LUMP_LIGHT_REGION_HULLS, BuildLightRegionHulls(*world, worldPrimaryLightCount));
        AddLump(lumps, LUMP_LIGHT_REGION_AXES, BuildLightRegionAxes(*world, worldPrimaryLightCount));

        if (gameWorldSp)
            AddLump(lumps, LUMP_PATHCONNECTIONS, BuildGameWorldSpPath(*gameWorldSp));

        const auto assetFile = context.OpenAssetFile(asset.m_name);
        if (assetFile)
            WriteBsp(*assetFile, std::move(lumps));
    }
} // namespace map_d3dbsp
