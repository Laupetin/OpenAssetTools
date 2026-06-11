#include "D3DBspLoaderIW3.h"

#include "Asset/AssetRegistration.h"
#include "D3DBspAssetCreationStateIW3.h"
#include "Game/IW3/CommonIW3.h"
#include "Game/IW3/Maps/D3DBspCommonIW3.h"
#include "Image/D3DFormat.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"
#include "Image/Texture.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <format>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace IW3;

namespace
{
    using enum IW3::d3dbsp::LumpType;
    using enum IW3::d3dbsp::TrisType;

    constexpr auto RAW_LIGHT_TYPE_OFFSET = 0uz;
    constexpr auto RAW_LIGHT_CAN_USE_SHADOW_MAP_OFFSET = 1uz;
    constexpr auto RAW_LIGHT_UNUSED_OFFSET = 2uz;
    constexpr auto RAW_LIGHT_COLOR_OFFSET = 4uz;
    constexpr auto RAW_LIGHT_DIR_OFFSET = 16uz;
    constexpr auto RAW_LIGHT_ORIGIN_OFFSET = 28uz;
    constexpr auto RAW_LIGHT_RADIUS_OFFSET = 40uz;
    constexpr auto RAW_LIGHT_COS_HALF_FOV_OUTER_OFFSET = 44uz;
    constexpr auto RAW_LIGHT_COS_HALF_FOV_INNER_OFFSET = 48uz;
    constexpr auto RAW_LIGHT_EXPONENT_OFFSET = 52uz;
    constexpr auto RAW_LIGHT_ROTATION_LIMIT_OFFSET = 56uz;
    constexpr auto RAW_LIGHT_TRANSLATION_LIMIT_OFFSET = 60uz;
    constexpr auto RAW_LIGHT_DEF_NAME_OFFSET = 64uz;
    constexpr auto RAW_LIGHT_DEF_NAME_SIZE = 64uz;
    constexpr auto RAW_MATERIAL_SIZE = 72uz;
    constexpr auto RAW_PLANE_SIZE = 16uz;
    constexpr auto RAW_BRUSHSIDE_SIZE = 8uz;
    constexpr auto RAW_BRUSH_HEADER_SIZE = 4uz;
    constexpr auto RAW_CLIP_NODE_SIZE = 36uz;
    constexpr auto RAW_LEAF_SIZE = 24uz;
    constexpr auto RAW_LEAF_CELL_INDEX_OFFSET = 20uz;
    constexpr auto RAW_LEAF_BRUSH_SIZE = 4uz;
    constexpr auto RAW_VEC3_SIZE = 12uz;
    constexpr auto RAW_TRI_INDICES_SIZE = 6uz;
    constexpr auto RAW_COLLISION_BORDER_SIZE = 28uz;
    constexpr auto RAW_COLLISION_PARTITION_SIZE = 12uz;
    constexpr auto RAW_COLLISION_AABB_SIZE = 32uz;
    constexpr auto RAW_MODEL_SIZE = 48uz;
    constexpr auto RAW_WORLD_SURFACE_SIZE = 24uz;
    constexpr auto RAW_WORLD_VERTEX_SIZE = 68uz;
    constexpr auto RAW_WORLD_AABB_TREE_SIZE = 12uz;
    constexpr auto RAW_WORLD_CELL_SIZE = 112uz;
    constexpr auto RAW_WORLD_PORTAL_SIZE = 16uz;
    constexpr auto RAW_LIGHTGRID_ENTRY_SIZE = sizeof(GfxLightGridEntry);
    constexpr auto RAW_LIGHTGRID_COLOR_SIZE = sizeof(GfxLightGridColors);
    constexpr auto RAW_LIGHT_REGION_HULL_SIZE = 76uz;
    constexpr auto RAW_LIGHT_REGION_AXIS_SIZE = sizeof(GfxLightRegionAxis);
    constexpr auto LIGHTMAP_PRIMARY_RAW_PAGE_SIZE = 0x100000uz;
    constexpr auto LIGHTMAP_SECONDARY_RAW_PAGE_SIZE = 0x200000uz;
    constexpr auto LIGHTMAP_RAW_PAGE_SIZE = LIGHTMAP_SECONDARY_RAW_PAGE_SIZE + LIGHTMAP_PRIMARY_RAW_PAGE_SIZE;
    constexpr auto LIGHTMAP_PRIMARY_RAW_WIDTH = 1024u;
    constexpr auto LIGHTMAP_PRIMARY_RAW_HEIGHT = 1024u;
    constexpr auto LIGHTMAP_SECONDARY_RAW_WIDTH = 512u;
    constexpr auto LIGHTMAP_SECONDARY_RAW_HEIGHT = 1024u;
    constexpr auto LIGHTMAP_SECONDARY_HALF_HEIGHT = LIGHTMAP_SECONDARY_RAW_HEIGHT / 2u;
    constexpr auto LIGHTMAP_SECONDARY_PIXEL_SIZE = 4u;
    constexpr auto REFLECTION_PROBE_SIZE = 64u;
    constexpr auto REFLECTION_PROBE_MIP_COUNT = 7u;
    constexpr auto REFLECTION_PROBE_NAME_SIZE = 64uz;
    constexpr auto REFLECTION_PROBE_RAW_DATA_SIZE = 0x1FFF8uz;
    constexpr auto REFLECTION_PROBE_RECORD_SIZE = sizeof(float) * 3uz + REFLECTION_PROBE_NAME_SIZE + REFLECTION_PROBE_RAW_DATA_SIZE;
    constexpr auto MATERIAL_USAGE_HASH_SIZE = 0x800uz;
    constexpr auto MATERIAL_HASH_SEARCH_SIZE = 0x7FFuz;
    constexpr auto MATERIAL_GAME_FLAG_MAGIC_PORTAL = 0x20u;
    constexpr auto DEFAULT_MATERIAL_NAME = "$default";
    constexpr auto DEFAULT_MATERIAL_REFERENCE_NAME = ",$default";
    constexpr auto SKY_LIGHTMAP_INDEX = 31u;
    constexpr auto MAX_LIGHTMAP_PAGE_COUNT = 31uz;
    constexpr auto PATHCONNECTIONS_VERSION = 8u;
    // linker_pc uses this exact float literal when converting entity angles to
    // radians. It is pi / 180, but the decompiled value avoids tiny ULP drift.
    constexpr auto DEG_TO_RAD = 0.01745329238474369f;

    [[nodiscard]] const IW3::d3dbsp::File* GetBspForAsset(const std::string& assetName, ISearchPath& searchPath, AssetCreationContext& context)
    {
        const auto& loadResult = context.GetZoneAssetCreationState<IW3::d3dbsp::AssetCreationState>().Load(assetName, searchPath);
        if (loadResult.status == IW3::d3dbsp::LoadStatus::NotFound)
            return nullptr;

        if (loadResult.status == IW3::d3dbsp::LoadStatus::Invalid)
        {
            con::error("Could not load d3dbsp for asset \"{}\": {}", assetName, loadResult.message);
            return nullptr;
        }

        return loadResult.file.get();
    }

    [[nodiscard]] bool BspWasInvalid(const std::string& assetName, ISearchPath& searchPath, AssetCreationContext& context)
    {
        const auto& loadResult = context.GetZoneAssetCreationState<IW3::d3dbsp::AssetCreationState>().Load(assetName, searchPath);
        return loadResult.status == IW3::d3dbsp::LoadStatus::Invalid;
    }

    [[nodiscard]] bool FitsInt(const size_t value)
    {
        return value <= static_cast<size_t>(std::numeric_limits<int>::max());
    }

    [[nodiscard]] bool FitsUnsigned(const size_t value)
    {
        return value <= static_cast<size_t>(std::numeric_limits<unsigned>::max());
    }

    [[nodiscard]] bool FitsUint16(const size_t value)
    {
        return value <= static_cast<size_t>(std::numeric_limits<uint16_t>::max());
    }

    [[nodiscard]] bool FitsInt16(const int value)
    {
        return value >= static_cast<int>(std::numeric_limits<int16_t>::min()) && value <= static_cast<int>(std::numeric_limits<int16_t>::max());
    }

    [[nodiscard]] float LinkerTrig(const float radians, const bool cosine)
    {
        // The stock x86 linker calls the double sin/cos functions and then
        // stores the result into float locals before building the axis.
        return static_cast<float>(cosine ? std::cos(static_cast<double>(radians)) : std::sin(static_cast<double>(radians)));
    }

    [[nodiscard]] float LinkerFloat(const double value)
    {
        // Make linker-style float spill points explicit. This keeps generated
        // quaternions closer to the 32-bit stock tool than pure float math.
        return static_cast<float>(value);
    }

    [[nodiscard]] unsigned HashAssetName(const char* name)
    {
        auto hash = 0u;
        if (!name)
            return hash;

        for (auto* pos = name; *pos; pos++)
            hash = static_cast<unsigned char>(*pos) ^ (33u * hash);

        return hash;
    }

    [[nodiscard]] std::optional<size_t> MaterialHashIndex(std::array<Material*, MATERIAL_USAGE_HASH_SIZE>& materialHashTable, Material* material)
    {
        if (!material || !material->info.name)
            return std::nullopt;

        // linker_pc resolves material usage through Material_GetHashIndex:
        // R_HashAssetName(name) % 0x7ff, then linear probe. Raw-loaded material
        // structs may have info.hashIndex == 0, so recompute the table here.
        const auto* name = material->info.name;
        auto hashIndex = static_cast<size_t>(HashAssetName(name)) % MATERIAL_HASH_SEARCH_SIZE;
        const auto beginHashIndex = hashIndex;
        do
        {
            auto* existingMaterial = materialHashTable[hashIndex];
            if (!existingMaterial)
            {
                materialHashTable[hashIndex] = material;
                return hashIndex;
            }

            if (existingMaterial == material || (existingMaterial->info.name && std::strcmp(existingMaterial->info.name, name) == 0))
                return hashIndex;

            hashIndex = (hashIndex + 1uz) % MATERIAL_HASH_SEARCH_SIZE;
        } while (hashIndex != beginHashIndex);

        return std::nullopt;
    }

    [[nodiscard]] float LinkerQuatSizeSq(const float (&candidate)[4], const size_t candidateIndex)
    {
        // Match the stock MatrixToQuat accumulation order for each candidate
        // before the testSizeSq float is compared against 1.0f.
        switch (candidateIndex)
        {
        case 0:
            return LinkerFloat(static_cast<double>(candidate[3]) * candidate[3] + static_cast<double>(candidate[1]) * candidate[1]
                               + static_cast<double>(candidate[0]) * candidate[0] + static_cast<double>(candidate[2]) * candidate[2]);

        case 1:
            return LinkerFloat(static_cast<double>(candidate[3]) * candidate[3] + static_cast<double>(candidate[2]) * candidate[2]
                               + static_cast<double>(candidate[0]) * candidate[0] + static_cast<double>(candidate[1]) * candidate[1]);

        case 2:
            return LinkerFloat(static_cast<double>(candidate[3]) * candidate[3] + static_cast<double>(candidate[2]) * candidate[2]
                               + static_cast<double>(candidate[0]) * candidate[0] + static_cast<double>(candidate[1]) * candidate[1]);

        default:
            return LinkerFloat(static_cast<double>(candidate[0]) * candidate[0] + static_cast<double>(candidate[1]) * candidate[1]
                               + static_cast<double>(candidate[2]) * candidate[2] + static_cast<double>(candidate[3]) * candidate[3]);
        }
    }

    [[nodiscard]] size_t NonSunPrimaryLightCount(const GfxWorld& world)
    {
        if (world.primaryLightCount <= world.sunPrimaryLightIndex + 1u)
            return 0uz;

        return world.primaryLightCount - world.sunPrimaryLightIndex - 1u;
    }

    [[nodiscard]] unsigned char U8(const char value)
    {
        return static_cast<unsigned char>(value);
    }

    [[nodiscard]] PackedUnitVec PackRawBspUnitVec(const float (&value)[3])
    {
        const auto packComponent = [](const float component)
        {
            const auto packed = static_cast<int>(static_cast<double>(component) * 127.0 + 127.5);
            return static_cast<uint8_t>(std::clamp(packed, 0, 255));
        };

        // Raw BSP vertex normals/tangents use the stock linker packer:
        // byte = int(component * 127.0 + 127.5), scale byte = 63.
        // Do not use the generic best-fit PackedUnitVec encoder here; it can
        // choose a different scale byte and will not dump back to linker_pc's
        // canonical world-vertex floats.
        return PackedUnitVec{static_cast<uint32_t>(packComponent(value[0]))
                             | (static_cast<uint32_t>(packComponent(value[1])) << 8u)
                             | (static_cast<uint32_t>(packComponent(value[2])) << 16u) | (63u << 24u)};
    }

    [[nodiscard]] const MaterialTechniqueSet* TechniqueSetForMaterial(const Material* material)
    {
        if (!material || !material->techniqueSet)
            return nullptr;

        return material->techniqueSet->remappedTechniqueSet ? material->techniqueSet->remappedTechniqueSet : material->techniqueSet;
    }

    [[nodiscard]] bool MaterialHasTechnique(const Material* material, const MaterialTechniqueType techniqueType)
    {
        const auto* techniqueSet = TechniqueSetForMaterial(material);
        if (!techniqueSet)
            return false;

        const auto index = static_cast<size_t>(techniqueType);
        if (index >= static_cast<size_t>(TECHNIQUE_COUNT))
            return false;

        return techniqueSet->techniques[index] != nullptr;
    }

    [[nodiscard]] unsigned char SamplerStateByte(const MaterialTextureDefSamplerState& samplerState)
    {
        return static_cast<unsigned char>((samplerState.filter & SAMPLER_FILTER_MASK)
                                          | ((samplerState.mipMap & SAMPLER_MIPMAP_COUNT) << SAMPLER_MIPMAP_SHIFT)
                                          | (samplerState.clampU ? SAMPLER_CLAMP_U : 0)
                                          | (samplerState.clampV ? SAMPLER_CLAMP_V : 0)
                                          | (samplerState.clampW ? SAMPLER_CLAMP_W : 0));
    }

    [[nodiscard]] bool ValidateRecordLump(
        const IW3::d3dbsp::File& bsp, const IW3::d3dbsp::Lump* lump, const IW3::d3dbsp::LumpType type, const size_t recordSize, std::string& error)
    {
        if (!lump)
        {
            error = std::format("missing lump {}", std::to_underlying(type));
            return false;
        }

        if (recordSize == 0uz || lump->data.size() % recordSize != 0uz)
        {
            error = std::format("{} lump {} has funny size {}", bsp.m_file_name, std::to_underlying(type), lump->data.size());
            return false;
        }

        return true;
    }

    [[nodiscard]] size_t RecordCount(const IW3::d3dbsp::Lump& lump, const size_t recordSize)
    {
        return recordSize > 0uz ? lump.data.size() / recordSize : 0uz;
    }

    template<typename T> T* AllocZeroed(MemoryManager& memory, const size_t count = 1uz)
    {
        auto* result = memory.Alloc<T>(count);
        if (result && count > 0uz)
            std::memset(result, 0, sizeof(T) * count);

        return result;
    }

    template<typename T> T* AllocCopy(MemoryManager& memory, const std::vector<std::byte>& data)
    {
        if (data.empty())
            return nullptr;

        auto* result = memory.Alloc<T>(data.size() / sizeof(T));
        std::memcpy(result, data.data(), data.size());
        return result;
    }

    template<typename T> void FillArray(T* data, const size_t count, const unsigned char value)
    {
        if (data && count > 0uz)
            std::memset(data, value, sizeof(T) * count);
    }

    template<typename T> void CopyUnaligned(const std::byte* source, T& destination)
    {
        std::memcpy(&destination, source, sizeof(T));
    }

    template<typename T> [[nodiscard]] T ReadUnaligned(const std::byte* source)
    {
        T result;
        std::memcpy(&result, source, sizeof(T));
        return result;
    }

    void CopyFloat3(const std::byte* source, float (&destination)[3])
    {
        std::memcpy(destination, source, sizeof(destination));
    }

    [[nodiscard]] char ReadRawByte(const std::byte* source, const size_t offset)
    {
        return static_cast<char>(std::to_integer<unsigned char>(source[offset]));
    }

    [[nodiscard]] uint32_t ReadU32(const std::byte* source, const size_t offset = 0uz)
    {
        return ReadUnaligned<uint32_t>(source + offset);
    }

    [[nodiscard]] int32_t ReadI32(const std::byte* source, const size_t offset = 0uz)
    {
        return ReadUnaligned<int32_t>(source + offset);
    }

    [[nodiscard]] uint16_t ReadU16(const std::byte* source, const size_t offset = 0uz)
    {
        return ReadUnaligned<uint16_t>(source + offset);
    }

    [[nodiscard]] float ReadFloat(const std::byte* source, const size_t offset = 0uz)
    {
        return ReadUnaligned<float>(source + offset);
    }

    [[nodiscard]] std::optional<float> ParseFloat(std::string_view value)
    {
        std::string temp(value);
        char* end = nullptr;
        const auto result = std::strtof(temp.c_str(), &end);
        if (end == temp.c_str())
            return std::nullopt;

        return result;
    }

    [[nodiscard]] int ParseInt(std::string_view value, const int fallback = 0)
    {
        std::string temp(value);
        char* end = nullptr;
        const auto result = std::strtol(temp.c_str(), &end, 10);
        if (end == temp.c_str())
            return fallback;

        return static_cast<int>(result);
    }

    [[nodiscard]] std::optional<std::array<float, 3>> ParseFloat3(std::string_view value)
    {
        std::array<float, 3> result{};
        std::string temp(value);
        const char* cursor = temp.c_str();

        for (auto i = 0uz; i < result.size(); i++)
        {
            while (*cursor && std::isspace(static_cast<unsigned char>(*cursor)))
                cursor++;

            char* end = nullptr;
            result[i] = std::strtof(cursor, &end);
            if (end == cursor)
                return std::nullopt;

            cursor = end;
        }

        return result;
    }

    [[nodiscard]] std::string RawString(const std::byte* data, const size_t maxLength)
    {
        auto length = 0uz;
        while (length < maxLength && data[length] != std::byte{})
            length++;

        return std::string(reinterpret_cast<const char*>(data), length);
    }

    [[nodiscard]] std::string RawMaterialName(const std::byte* record)
    {
        return RawString(record, 64uz);
    }

    [[nodiscard]] bool HasPathSeparator(const std::string_view value)
    {
        return value.find('/') != std::string_view::npos || value.find('\\') != std::string_view::npos;
    }

    [[nodiscard]] std::string BspBaseName(std::string_view assetName)
    {
        const auto lastSlash = assetName.find_last_of("/\\");
        auto baseName = lastSlash == std::string_view::npos ? assetName : assetName.substr(lastSlash + 1uz);

        constexpr std::string_view BSP_EXTENSION = ".d3dbsp";
        if (baseName.size() >= BSP_EXTENSION.size() && baseName.substr(baseName.size() - BSP_EXTENSION.size()) == BSP_EXTENSION)
            baseName.remove_suffix(BSP_EXTENSION.size());

        return std::string(baseName);
    }

    void CrossProduct(const float (&left)[3], const float (&right)[3], float (&out)[3])
    {
        out[0] = left[1] * right[2] - left[2] * right[1];
        out[1] = left[2] * right[0] - left[0] * right[2];
        out[2] = left[0] * right[1] - left[1] * right[0];
    }

    [[nodiscard]] float DotProduct(const float (&left)[3], const float (&right)[3])
    {
        return left[0] * right[0] + left[1] * right[1] + left[2] * right[2];
    }

    [[nodiscard]] float LengthSquared(const float (&value)[3])
    {
        return DotProduct(value, value);
    }

    void Normalize(float (&value)[3])
    {
        const auto lengthSq = LengthSquared(value);
        if (lengthSq <= 0.0f)
            return;

        const auto invLength = 1.0f / std::sqrt(lengthSq);
        for (auto axis = 0uz; axis < 3uz; axis++)
            value[axis] *= invLength;
    }

    void PerpendicularVector(const float (&source)[3], float (&destination)[3])
    {
        auto bestAxis = 0uz;
        auto bestAbs = std::fabs(source[0]);
        for (auto axis = 1uz; axis < 3uz; axis++)
        {
            const auto currentAbs = std::fabs(source[axis]);
            if (currentAbs < bestAbs)
            {
                bestAxis = axis;
                bestAbs = currentAbs;
            }
        }

        float temp[3]{};
        temp[bestAxis] = 1.0f;
        const auto projection = DotProduct(temp, source);
        for (auto axis = 0uz; axis < 3uz; axis++)
            destination[axis] = temp[axis] - projection * source[axis];
        Normalize(destination);
    }

    [[nodiscard]] uint8_t ClampToByte(const int value)
    {
        return static_cast<uint8_t>(std::clamp(value, 0, 255));
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
        constexpr double BLACK_LEVEL = 0.3;
        constexpr double WHITE_LEVEL = 0.7;
        constexpr double GAMMA = 1.2;
        constexpr double SATURATION = 0.5;

        const auto range = WHITE_LEVEL - BLACK_LEVEL;
        double color[3]{
            std::pow(std::max(0.0, (static_cast<double>(r) / 255.0 - BLACK_LEVEL) / range), GAMMA),
            std::pow(std::max(0.0, (static_cast<double>(g) / 255.0 - BLACK_LEVEL) / range), GAMMA),
            std::pow(std::max(0.0, (static_cast<double>(b) / 255.0 - BLACK_LEVEL) / range), GAMMA),
        };

        const auto luma = LUMA_R * color[0] + LUMA_G * color[1] + LUMA_B * color[2];
        for (auto& channel : color)
            channel = SATURATION * channel + (1.0 - SATURATION) * luma;

        const auto scale = std::max({0.1, color[0], color[1], color[2]});
        return PackRgba(ClampToByte(static_cast<int>(color[0] / scale * 255.0)),
                        ClampToByte(static_cast<int>(color[1] / scale * 255.0)),
                        ClampToByte(static_cast<int>(color[2] / scale * 255.0)),
                        ClampToByte(static_cast<int>(255.0 * (scale * 0.25))));
    }

    [[nodiscard]] std::optional<uint8_t> ParseHexByte(const std::string_view value, const size_t offset)
    {
        if (offset + 2uz > value.size())
            return std::nullopt;

        const auto hexValue = [](const char c) -> int
        {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
            return -1;
        };

        const auto high = hexValue(value[offset]);
        const auto low = hexValue(value[offset + 1uz]);
        if (high < 0 || low < 0)
            return std::nullopt;

        return static_cast<uint8_t>((high << 4) | low);
    }

    [[nodiscard]] std::string GeneratedImageName(const std::string& assetName, const std::string_view kind, const size_t index)
    {
        return std::format("{}_{}_{}", assetName, kind, index);
    }

    [[nodiscard]] std::string OutdoorImageName()
    {
        return "$outdoor";
    }

    [[nodiscard]] GfxImageLoadDef* CreateLoadDef(
        MemoryManager& memory, const uint16_t width, const uint16_t height, const uint16_t depth, const int format, const char flags, const std::byte* data, const size_t dataSize)
    {
        auto* loadDef = static_cast<GfxImageLoadDef*>(memory.AllocRaw(offsetof(GfxImageLoadDef, data) + dataSize));
        loadDef->levelCount = (flags & image::iwi6::IMG_FLAG_CUBEMAP) != 0 ? static_cast<char>(REFLECTION_PROBE_MIP_COUNT) : 1;
        loadDef->flags = flags;
        loadDef->dimensions[0] = width;
        loadDef->dimensions[1] = height;
        loadDef->dimensions[2] = depth;
        loadDef->format = format;
        loadDef->resourceSize = static_cast<unsigned>(dataSize);
        if (data && dataSize > 0uz)
            std::memcpy(loadDef->data, data, dataSize);

        return loadDef;
    }

    [[nodiscard]] GfxImage* CreateGeneratedImage(
        MemoryManager& memory,
        const std::string& name,
        const MapType mapType,
        const TextureSemantic semantic,
        const ImageCategory category,
        const uint16_t width,
        const uint16_t height,
        const uint16_t depth,
        const int format,
        const char loadFlags,
        const std::byte* data,
        const size_t dataSize)
    {
        auto* image = AllocZeroed<GfxImage>(memory);
        image->name = memory.Dup(name.c_str());
        image->mapType = mapType;
        image->noPicmip = true;
        image->semantic = static_cast<char>(semantic);
        image->category = static_cast<char>(category);
        image->width = width;
        image->height = height;
        image->depth = depth;
        image->delayLoadPixels = true;
        image->texture.loadDef = CreateLoadDef(memory, width, height, depth, format, loadFlags, data, dataSize);
        return image;
    }

    struct EntityBlock
    {
        std::string text;
        std::string classname;
        std::unordered_map<std::string, std::string> fields;
    };

    struct RawLightPixel
    {
        uint8_t r = 0u;
        uint8_t g = 0u;
        uint8_t b = 0u;
        uint8_t a = 255u;
    };

    [[nodiscard]] float CosOfSumOfArcCos(const float cos0, const float cos1)
    {
        return cos0 * cos1 - std::sqrt((1.0f - cos0 * cos0) * (1.0f - cos1 * cos1));
    }

    void ParsePrimaryLightRecord(const std::byte* record, ComPrimaryLight& light, MemoryManager& memory)
    {
        light = {};
        light.type = ReadRawByte(record, RAW_LIGHT_TYPE_OFFSET);
        light.canUseShadowMap = ReadRawByte(record, RAW_LIGHT_CAN_USE_SHADOW_MAP_OFFSET);
        light.exponent = static_cast<char>(ReadI32(record, RAW_LIGHT_EXPONENT_OFFSET));
        light.unused = 0;

        CopyFloat3(record + RAW_LIGHT_COLOR_OFFSET, light.color);
        CopyFloat3(record + RAW_LIGHT_DIR_OFFSET, light.dir);
        CopyFloat3(record + RAW_LIGHT_ORIGIN_OFFSET, light.origin);
        CopyUnaligned(record + RAW_LIGHT_RADIUS_OFFSET, light.radius);
        CopyUnaligned(record + RAW_LIGHT_COS_HALF_FOV_OUTER_OFFSET, light.cosHalfFovOuter);
        CopyUnaligned(record + RAW_LIGHT_COS_HALF_FOV_INNER_OFFSET, light.cosHalfFovInner);
        CopyUnaligned(record + RAW_LIGHT_ROTATION_LIMIT_OFFSET, light.rotationLimit);
        CopyUnaligned(record + RAW_LIGHT_TRANSLATION_LIMIT_OFFSET, light.translationLimit);

        // IW3 v22 stores a DiskPrimaryLight. The linker normalises this into a
        // runtime ComPrimaryLight, including inner-FOV correction and derived
        // expanded FOV for spot/omni lights.
        if (light.type >= 2)
        {
            const auto defName = RawString(record + RAW_LIGHT_DEF_NAME_OFFSET, RAW_LIGHT_DEF_NAME_SIZE);
            light.defName = defName.empty() ? nullptr : memory.Dup(defName.c_str());

            if (light.cosHalfFovOuter >= light.cosHalfFovInner)
                light.cosHalfFovInner = light.cosHalfFovOuter * 0.75f + 0.25f;

            if (light.rotationLimit == 1.0f)
                light.cosHalfFovExpanded = light.cosHalfFovOuter;
            else if (-light.cosHalfFovOuter < light.rotationLimit)
                light.cosHalfFovExpanded = CosOfSumOfArcCos(light.cosHalfFovOuter, light.rotationLimit);
            else
                light.cosHalfFovExpanded = -1.0f;
        }
        else
        {
            light.defName = nullptr;
            light.cosHalfFovExpanded = light.cosHalfFovOuter;
        }
    }

    [[nodiscard]] std::string LowercaseAscii(std::string value)
    {
        for (auto& c : value)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        return value;
    }

    [[nodiscard]] std::string AssetNameWithoutReferencePrefix(const std::string_view assetName)
    {
        if (!assetName.empty() && assetName.front() == ',')
            return std::string(assetName.substr(1));

        return std::string(assetName);
    }

    [[nodiscard]] uint8_t ReadUnsignedChannel(const uint64_t pixel, const unsigned offset, const unsigned size, const uint8_t fallback)
    {
        if (size == 0u)
            return fallback;

        const auto maxValue = (1ull << size) - 1ull;
        const auto value = (pixel >> offset) & maxValue;
        if (size == 8u)
            return static_cast<uint8_t>(value);

        return static_cast<uint8_t>((value * 255ull + maxValue / 2ull) / maxValue);
    }

    [[nodiscard]] RawLightPixel ReadRawLightPixel(const uint8_t* buffer, const image::ImageFormatUnsigned& format, const size_t pixelIndex)
    {
        const auto bytesPerPixel = format.m_bits_per_pixel / 8u;

        uint64_t pixel = 0u;
        for (auto byteIndex = 0u; byteIndex < bytesPerPixel; byteIndex++)
            pixel |= static_cast<uint64_t>(buffer[pixelIndex * bytesPerPixel + byteIndex]) << (byteIndex * 8u);

        RawLightPixel result{};
        result.r = ReadUnsignedChannel(pixel, format.m_r_offset, format.m_r_size, 0u);
        result.g = format.HasG() ? ReadUnsignedChannel(pixel, format.m_g_offset, format.m_g_size, 0u) : result.r;
        result.b = format.HasB() ? ReadUnsignedChannel(pixel, format.m_b_offset, format.m_b_size, 0u) : result.r;
        result.a = ReadUnsignedChannel(pixel, format.m_a_offset, format.m_a_size, 255u);
        return result;
    }

    [[nodiscard]] bool LoadStockLightDefAttenuationPixels(const std::string& imageName, std::vector<RawLightPixel>& pixels)
    {
        if (LowercaseAscii(imageName) != "falloff_linear")
            return false;

        // Stock CoD4 light_point_linear references falloff_linear. Some build
        // setups resolve the lightdef as a referenced asset without exposing the
        // IWI to OAT's search path, so keep the exact stock 32x1 L8 top mip here
        // as a narrow fallback. linker_pc samples this and writes the falloff
        // strip into the generated secondary lightmap atlas.
        constexpr std::array<uint8_t, 32> FALLBACK_FALLOFF_LINEAR{
            255u, 255u, 255u, 254u, 250u, 244u, 235u, 225u, 215u, 205u, 195u, 186u, 175u, 167u, 155u, 146u,
            136u, 126u, 117u, 108u, 98u,  87u,  78u,  66u,  58u,  49u,  39u,  28u,  19u,  10u,  4u,   0u,
        };

        pixels.clear();
        pixels.reserve(FALLBACK_FALLOFF_LINEAR.size());
        for (const auto value : FALLBACK_FALLOFF_LINEAR)
        {
            pixels.emplace_back(RawLightPixel{
                .r = value,
                .g = value,
                .b = value,
                .a = 255u,
            });
        }

        return true;
    }

    [[nodiscard]] bool LoadLightDefAttenuationPixels(const GfxLightDef& lightDef, ISearchPath& searchPath, std::vector<RawLightPixel>& pixels, std::string& error)
    {
        const auto* image = lightDef.attenuation.image;
        if (!image || !image->name)
        {
            error = std::format("light def \"{}\" has no attenuation image", lightDef.name ? lightDef.name : "");
            return false;
        }

        const auto imageName = AssetNameWithoutReferencePrefix(image->name);
        const auto file = searchPath.Open(image::GetFileNameForAsset(imageName, ".iwi"));
        if (!file.IsOpen())
        {
            if (LoadStockLightDefAttenuationPixels(imageName, pixels))
                return true;

            error = std::format("missing attenuation image \"{}\" for light def \"{}\"", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        auto loadResult = image::LoadIwi(*file.m_stream);
        if (!loadResult || !loadResult->m_texture)
        {
            error = std::format("could not load attenuation image \"{}\" for light def \"{}\"", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        const auto& texture = *loadResult->m_texture;
        if (texture.GetTextureType() != image::TextureType::T_2D)
        {
            error = std::format("attenuation image \"{}\" for light def \"{}\" is not a 2D image", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        const auto* format = texture.GetFormat();
        if (!format || format->GetType() != image::ImageFormatType::UNSIGNED)
        {
            error = std::format("attenuation image \"{}\" for light def \"{}\" has unsupported format", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        const auto* unsignedFormat = dynamic_cast<const image::ImageFormatUnsigned*>(format);
        if (!unsignedFormat || unsignedFormat->m_bits_per_pixel == 0u || unsignedFormat->m_bits_per_pixel % 8u != 0u
            || unsignedFormat->m_bits_per_pixel > 64u)
        {
            error = std::format("attenuation image \"{}\" for light def \"{}\" has unsupported pixel size", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        const auto* buffer = texture.GetBufferForMipLevel(0);
        if (!buffer)
        {
            error = std::format("attenuation image \"{}\" for light def \"{}\" has no pixels", imageName, lightDef.name ? lightDef.name : "");
            return false;
        }

        pixels.clear();
        pixels.reserve(texture.GetWidth());
        for (auto x = 0u; x < texture.GetWidth(); x++)
            pixels.emplace_back(ReadRawLightPixel(buffer, *unsignedFormat, x));

        return true;
    }

    void WriteSecondaryLightmapPixel(std::vector<std::byte>& out, const size_t pixelOffset, const RawLightPixel& pixel)
    {
        const auto byteOffset = pixelOffset * LIGHTMAP_SECONDARY_PIXEL_SIZE;
        out[byteOffset + 0uz] = static_cast<std::byte>(pixel.b);
        out[byteOffset + 1uz] = static_cast<std::byte>(pixel.g);
        out[byteOffset + 2uz] = static_cast<std::byte>(pixel.r);
        out[byteOffset + 3uz] = static_cast<std::byte>(pixel.a);
    }

    [[nodiscard]] RawLightPixel LerpLightPixel(const RawLightPixel& current, const RawLightPixel& next, const unsigned zoom, const unsigned lerp)
    {
        const auto scale = 2u * zoom;
        const auto currentScale = scale - lerp;

        const auto lerpChannel = [zoom, lerp, currentScale, scale](const uint8_t currentChannel, const uint8_t nextChannel)
        {
            return static_cast<uint8_t>((zoom + lerp * static_cast<unsigned>(nextChannel) + currentScale * static_cast<unsigned>(currentChannel)) / scale);
        };

        return RawLightPixel{
            .r = lerpChannel(current.r, next.r),
            .g = lerpChannel(current.g, next.g),
            .b = lerpChannel(current.b, next.b),
            .a = lerpChannel(current.a, next.a),
        };
    }

    [[nodiscard]] std::vector<std::string> QuotedEntityTokens(const std::string& block)
    {
        std::vector<std::string> tokens;
        for (auto i = 0uz; i < block.size(); i++)
        {
            if (block[i] != '"')
                continue;

            std::string token;
            bool escape = false;
            for (++i; i < block.size(); i++)
            {
                const auto c = block[i];
                if (escape)
                {
                    token.push_back(c);
                    escape = false;
                }
                else if (c == '\\')
                {
                    escape = true;
                }
                else if (c == '"')
                {
                    break;
                }
                else
                {
                    token.push_back(c);
                }
            }

            tokens.emplace_back(std::move(token));
        }

        return tokens;
    }

    [[nodiscard]] std::unordered_map<std::string, std::string> QuotedEntityFields(const std::string& block)
    {
        const auto tokens = QuotedEntityTokens(block);
        std::unordered_map<std::string, std::string> fields;
        fields.reserve(tokens.size() / 2uz);

        for (auto i = 0uz; i + 1uz < tokens.size(); i += 2uz)
            fields.emplace(tokens[i], tokens[i + 1uz]);

        return fields;
    }

    [[nodiscard]] std::string_view EntityField(const EntityBlock& block, const std::string& key)
    {
        const auto existingField = block.fields.find(key);
        if (existingField == block.fields.end())
            return {};

        return existingField->second;
    }

    [[nodiscard]] bool ParseEntityBlocks(const std::vector<std::byte>& lump, std::vector<EntityBlock>& blocks, std::string& error)
    {
        auto textLen = lump.size();
        while (textLen > 0uz && lump[textLen - 1uz] == std::byte{})
            textLen--;

        const std::string text(reinterpret_cast<const char*>(lump.data()), textLen);
        auto offset = 0uz;

        while (offset < text.size())
        {
            while (offset < text.size() && text[offset] != '{')
            {
                if (!std::isspace(static_cast<unsigned char>(text[offset])))
                {
                    error = "unexpected non-whitespace before entity block";
                    return false;
                }
                offset++;
            }

            if (offset >= text.size())
                break;

            const auto blockStart = offset;
            auto depth = 1;
            auto inQuote = false;
            auto escape = false;

            for (++offset; offset < text.size(); offset++)
            {
                const auto c = text[offset];
                if (inQuote)
                {
                    if (escape)
                        escape = false;
                    else if (c == '\\')
                        escape = true;
                    else if (c == '"')
                        inQuote = false;
                }
                else
                {
                    if (c == '"')
                        inQuote = true;
                    else if (c == '{')
                        depth++;
                    else if (c == '}')
                    {
                        depth--;
                        if (depth == 0)
                        {
                            offset++;
                            if (offset < text.size() && text[offset] == '\r')
                                offset++;
                            if (offset < text.size() && text[offset] == '\n')
                                offset++;

                            EntityBlock block;
                            block.text = text.substr(blockStart, offset - blockStart);
                            block.fields = QuotedEntityFields(block.text);
                            block.classname = std::string(EntityField(block, "classname"));
                            blocks.emplace_back(std::move(block));
                            break;
                        }
                    }
                }
            }

            if (depth != 0)
            {
                error = "unterminated entity block";
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool IsLinkerConsumedMapEntity(const EntityBlock& block)
    {
        // linker_pc removes these editor/compiler entities from runtime
        // MapEnts while building the corresponding static model, dynamic
        // entity, reflection probe, prefab, or helper data.
        if (block.classname == "misc_model" || block.classname == "misc_prefab" || block.classname == "dyn_brushmodel" || block.classname == "dyn_model"
            || block.classname == "reflection_probe" || block.classname == "info_null" || block.classname == "func_group")
        {
            return true;
        }

        // linker_pc keeps primary-light entities only when the entity was tagged
        // with a generated pl# key. Uncompiled light entities are consumed by the
        // primary-light path and are not retained in runtime MapEnts.
        if (block.classname == "light" && EntityField(block, "pl#").empty())
            return true;

        return false;
    }

    [[nodiscard]] std::vector<std::byte> CompileMapEntsEntityString(const std::vector<EntityBlock>& blocks)
    {
        std::string out;
        for (const auto& block : blocks)
        {
            if (IsLinkerConsumedMapEntity(block))
                continue;

            out += block.text;
        }

        if (out.size() >= 2uz && out[out.size() - 2uz] == '\r' && out[out.size() - 1uz] == '\n')
            out.resize(out.size() - 2uz);
        else if (!out.empty() && (out.back() == '\n' || out.back() == '\r'))
            out.pop_back();

        std::vector<std::byte> result;
        result.reserve(out.size() + 1uz);
        const auto* outBytes = reinterpret_cast<const std::byte*>(out.data());
        result.insert(result.end(), outBytes, outBytes + out.size());
        result.emplace_back(std::byte{});
        return result;
    }

    [[nodiscard]] bool IsMiscModel(const EntityBlock& block)
    {
        return block.classname == "misc_model";
    }

    [[nodiscard]] bool IsDynModel(const EntityBlock& block)
    {
        return block.classname == "dyn_model";
    }

    [[nodiscard]] DynEntityType DynEntityTypeFromString(const std::string_view value)
    {
        if (value.empty() || value == "clutter")
            return DYNENT_TYPE_CLUTTER;

        if (value == "destruct")
            return DYNENT_TYPE_DESTRUCT;

        return DYNENT_TYPE_INVALID;
    }

    [[nodiscard]] float StaticModelScale(const EntityBlock& block)
    {
        const auto parsedScale = ParseFloat(EntityField(block, "modelscale"));
        if (!parsedScale || *parsedScale <= std::numeric_limits<float>::epsilon())
            return 1.0f;

        return *parsedScale;
    }

    void AnglesToAxis(const std::array<float, 3>& angles, float (&axis)[3][3])
    {
        const auto pitch = angles[0] * DEG_TO_RAD;
        const auto yaw = angles[1] * DEG_TO_RAD;
        const auto roll = angles[2] * DEG_TO_RAD;

        const auto sp = LinkerTrig(pitch, false);
        const auto cp = LinkerTrig(pitch, true);
        const auto sy = LinkerTrig(yaw, false);
        const auto cy = LinkerTrig(yaw, true);
        const auto sr = LinkerTrig(roll, false);
        const auto cr = LinkerTrig(roll, true);

        axis[0][0] = LinkerFloat(static_cast<double>(cp) * cy);
        axis[0][1] = LinkerFloat(static_cast<double>(cp) * sy);
        axis[0][2] = -sp;

        const auto srSp = LinkerFloat(static_cast<double>(sr) * sp);
        axis[1][0] = LinkerFloat(static_cast<double>(cy) * srSp - static_cast<double>(cr) * sy);
        axis[1][1] = LinkerFloat(static_cast<double>(srSp) * sy + static_cast<double>(cr) * cy);
        axis[1][2] = LinkerFloat(static_cast<double>(sr) * cp);

        const auto crSp = LinkerFloat(static_cast<double>(cr) * sp);
        axis[2][0] = LinkerFloat(static_cast<double>(cy) * crSp + static_cast<double>(sy) * sr);
        axis[2][1] = LinkerFloat(static_cast<double>(sy) * crSp - static_cast<double>(cy) * sr);
        axis[2][2] = LinkerFloat(static_cast<double>(cr) * cp);
    }

    void AxisToQuat(const float (&axis)[3][3], float (&quat)[4])
    {
        // Match linker_pc's Com_Math MatrixToQuat path. It tests quaternion
        // candidates in this fixed order, normalizes the first candidate with
        // sizeSq >= 1, and intentionally preserves the resulting sign.
        float candidates[4][4]{};
        candidates[0][0] = LinkerFloat(static_cast<double>(axis[1][2]) - axis[2][1]);
        candidates[0][1] = LinkerFloat(static_cast<double>(axis[2][0]) - axis[0][2]);
        candidates[0][2] = LinkerFloat(static_cast<double>(axis[0][1]) - axis[1][0]);
        candidates[0][3] = LinkerFloat(static_cast<double>(axis[1][1]) + axis[0][0] + axis[2][2] + 1.0);

        candidates[1][0] = LinkerFloat(static_cast<double>(axis[2][0]) + axis[0][2]);
        candidates[1][1] = LinkerFloat(static_cast<double>(axis[2][1]) + axis[1][2]);
        candidates[1][2] = LinkerFloat(static_cast<double>(axis[2][2]) - axis[1][1] - axis[0][0] + 1.0);
        candidates[1][3] = candidates[0][2];

        candidates[2][0] = LinkerFloat(static_cast<double>(axis[0][0]) - axis[1][1] - axis[2][2] + 1.0);
        candidates[2][1] = LinkerFloat(static_cast<double>(axis[1][0]) + axis[0][1]);
        candidates[2][2] = candidates[1][0];
        candidates[2][3] = candidates[0][0];

        candidates[3][0] = candidates[2][1];
        candidates[3][1] = LinkerFloat(static_cast<double>(axis[1][1]) - axis[0][0] - axis[2][2] + 1.0);
        candidates[3][2] = candidates[1][1];
        candidates[3][3] = candidates[0][1];

        auto selectedCandidate = 3uz;
        auto selectedSizeSq = 0.0f;

        for (auto candidateIndex = 0uz; candidateIndex < 4uz; candidateIndex++)
        {
            const auto sizeSq = LinkerQuatSizeSq(candidates[candidateIndex], candidateIndex);

            selectedCandidate = candidateIndex;
            selectedSizeSq = sizeSq;

            if (sizeSq >= 1.0f)
                break;
        }

        const auto sqrtSize = LinkerFloat(std::sqrt(static_cast<double>(selectedSizeSq)));
        const auto scale = LinkerFloat(1.0 / sqrtSize);
        for (auto component = 0uz; component < 4uz; component++)
            quat[component] = LinkerFloat(static_cast<double>(candidates[selectedCandidate][component]) * scale);
    }

    void TransformStaticModelPoint(
        const float (&axis)[3][3], const std::array<float, 3>& origin, const float scale, const float x, const float y, const float z, float (&out)[3])
    {
        for (auto component = 0uz; component < 3uz; component++)
            out[component] = origin[component] + scale * (axis[0][component] * x + axis[1][component] * y + axis[2][component] * z);
    }

    void BuildStaticModelBounds(const XModel& model, const float (&axis)[3][3], const std::array<float, 3>& origin, const float scale, cStaticModel_s& out)
    {
        for (auto component = 0uz; component < 3uz; component++)
        {
            out.absmin[component] = std::numeric_limits<float>::max();
            out.absmax[component] = std::numeric_limits<float>::lowest();
        }

        for (auto corner = 0u; corner < 8u; corner++)
        {
            const auto x = (corner & 1u) != 0u ? model.maxs.x : model.mins.x;
            const auto y = (corner & 2u) != 0u ? model.maxs.y : model.mins.y;
            const auto z = (corner & 4u) != 0u ? model.maxs.z : model.mins.z;

            float transformed[3]{};
            TransformStaticModelPoint(axis, origin, scale, x, y, z, transformed);
            for (auto component = 0uz; component < 3uz; component++)
            {
                out.absmin[component] = std::min(out.absmin[component], transformed[component]);
                out.absmax[component] = std::max(out.absmax[component], transformed[component]);
            }
        }
    }

    [[nodiscard]] std::vector<const EntityBlock*> StaticModelEntityBlocks(const std::vector<EntityBlock>& blocks)
    {
        std::vector<const EntityBlock*> result;
        for (const auto& block : blocks)
        {
            if (IsMiscModel(block))
                result.emplace_back(&block);
        }

        return result;
    }

    [[nodiscard]] std::vector<XAssetInfo<XModel>*> LoadStaticModelDependencies(
        const std::vector<const EntityBlock*>& staticModelBlocks, AssetCreationContext& context)
    {
        std::vector<XAssetInfo<XModel>*> result;
        result.reserve(staticModelBlocks.size());

        for (const auto* block : staticModelBlocks)
        {
            const auto modelName = EntityField(*block, "model");
            if (modelName.empty())
            {
                result.emplace_back(nullptr);
                continue;
            }

            result.emplace_back(context.LoadDependency<AssetXModel>(std::string(modelName)));
        }

        return result;
    }

    struct DynModelDependency
    {
        XAssetInfo<XModel>* model;
        XAssetInfo<PhysPreset>* physPreset;
    };

    [[nodiscard]] std::vector<const EntityBlock*> DynModelEntityBlocks(const std::vector<EntityBlock>& blocks)
    {
        std::vector<const EntityBlock*> result;
        for (const auto& block : blocks)
        {
            if (IsDynModel(block))
                result.emplace_back(&block);
        }

        return result;
    }

    [[nodiscard]] std::vector<DynModelDependency> LoadDynModelDependencies(const std::vector<const EntityBlock*>& dynModelBlocks, AssetCreationContext& context)
    {
        std::vector<DynModelDependency> result;
        result.reserve(dynModelBlocks.size());

        for (const auto* block : dynModelBlocks)
        {
            DynModelDependency dependency{};

            const auto modelName = EntityField(*block, "model");
            if (!modelName.empty())
                dependency.model = context.LoadDependency<AssetXModel>(std::string(modelName));

            const auto physPresetName = EntityField(*block, "physPreset");
            if (!physPresetName.empty())
                dependency.physPreset = context.LoadDependency<AssetPhysPreset>(std::string(physPresetName));

            result.emplace_back(dependency);
        }

        return result;
    }

    [[nodiscard]] PhysPreset* ResolveDynModelPhysPreset(const DynModelDependency& dependency, AssetCreationContext& context)
    {
        if (dependency.physPreset)
            return dependency.physPreset->Asset();

        if (dependency.model && dependency.model->Asset()->physPreset)
            return dependency.model->Asset()->physPreset;

        // linker_pc falls back to "default" when neither the entity nor the
        // model provides a physics preset.
        auto* defaultPreset = context.LoadDependency<AssetPhysPreset>("default");
        return defaultPreset ? defaultPreset->Asset() : nullptr;
    }

    void PopulateDynModelMass(const EntityBlock& block, DynEntityDef& dynEnt)
    {
        if (const auto centerOfMass = ParseFloat3(EntityField(block, "centerofmass")))
            std::copy(centerOfMass->begin(), centerOfMass->end(), dynEnt.mass.centerOfMass);

        if (const auto momentsOfInertia = ParseFloat3(EntityField(block, "momofinertia")))
            std::copy(momentsOfInertia->begin(), momentsOfInertia->end(), dynEnt.mass.momentsOfInertia);

        if (const auto productsOfInertia = ParseFloat3(EntityField(block, "prodofinertia")))
            std::copy(productsOfInertia->begin(), productsOfInertia->end(), dynEnt.mass.productsOfInertia);
    }

    void PopulateStaticModels(
        clipMap_t& clipMap,
        const std::vector<const EntityBlock*>& staticModelBlocks,
        const std::vector<XAssetInfo<XModel>*>& staticModelDependencies,
        MemoryManager& memory)
    {
        std::vector<std::pair<const EntityBlock*, XModel*>> validStaticModels;
        validStaticModels.reserve(staticModelBlocks.size());

        for (auto i = 0uz; i < staticModelBlocks.size(); i++)
        {
            if (i >= staticModelDependencies.size() || !staticModelDependencies[i])
                continue;

            validStaticModels.emplace_back(staticModelBlocks[i], staticModelDependencies[i]->Asset());
        }

        if (validStaticModels.empty())
            return;

        clipMap.numStaticModels = static_cast<unsigned>(validStaticModels.size());
        clipMap.staticModelList = AllocZeroed<cStaticModel_s>(memory, validStaticModels.size());

        for (auto modelIndex = 0uz; modelIndex < validStaticModels.size(); modelIndex++)
        {
            const auto& [block, model] = validStaticModels[modelIndex];
            auto& staticModel = clipMap.staticModelList[modelIndex];
            const auto origin = ParseFloat3(EntityField(*block, "origin")).value_or(std::array<float, 3>{});
            const auto angles = ParseFloat3(EntityField(*block, "angles")).value_or(std::array<float, 3>{});
            const auto scale = StaticModelScale(*block);
            float axis[3][3]{};

            AnglesToAxis(angles, axis);

            staticModel.writable.nextModelInWorldSector = std::numeric_limits<uint16_t>::max();
            staticModel.xmodel = model;
            std::copy(origin.begin(), origin.end(), staticModel.origin);

            // Runtime cStaticModel_s stores the transpose of the model axis divided
            // by scale. The raw BSP stores editor angles/modelscale, so this is the
            // inverse of D3DBspDumperIW3::StaticModelAxis/StaticModelScale.
            for (auto row = 0uz; row < 3uz; row++)
            {
                for (auto column = 0uz; column < 3uz; column++)
                    staticModel.invScaledAxis[column][row] = axis[row][column] / scale;
            }

            BuildStaticModelBounds(*model, axis, origin, scale, staticModel);
        }
    }

    void PopulateDynModelEntities(
        clipMap_t& clipMap,
        const std::vector<const EntityBlock*>& dynModelBlocks,
        const std::vector<DynModelDependency>& dynModelDependencies,
        AssetCreationContext& context,
        MemoryManager& memory)
    {
        struct DynModelBuildEntry
        {
            const EntityBlock* block;
            XModel* model;
            PhysPreset* physPreset;
            size_t sourceIndex;
        };

        std::vector<DynModelBuildEntry> validDynModels;
        validDynModels.reserve(dynModelBlocks.size());

        for (auto i = 0uz; i < dynModelBlocks.size(); i++)
        {
            if (i >= dynModelDependencies.size() || !dynModelDependencies[i].model)
                continue;

            const auto type = DynEntityTypeFromString(EntityField(*dynModelBlocks[i], "type"));
            if (type == DYNENT_TYPE_INVALID)
                continue;

            validDynModels.emplace_back(dynModelBlocks[i], dynModelDependencies[i].model->Asset(), ResolveDynModelPhysPreset(dynModelDependencies[i], context), i);
        }

        if (validDynModels.empty())
            return;

        // linker_pc sorts dynents by runtime xmodel pointer before splitting the
        // model/brush arrays. Pointer order depends on linker asset allocation,
        // not the BSP payload, so OAT writes a deterministic canonical order.
        std::stable_sort(validDynModels.begin(),
                         validDynModels.end(),
                         [](const DynModelBuildEntry& left, const DynModelBuildEntry& right)
                         {
                             const auto leftName = left.model && left.model->name ? left.model->name : "";
                             const auto rightName = right.model && right.model->name ? right.model->name : "";
                             const auto nameCompare = std::strcmp(leftName, rightName);
                             if (nameCompare != 0)
                                 return nameCompare < 0;

                             return left.sourceIndex < right.sourceIndex;
                         });

        const auto count = std::min(validDynModels.size(), static_cast<size_t>(std::numeric_limits<uint16_t>::max()));
        clipMap.dynEntCount[0] = static_cast<uint16_t>(count);
        clipMap.dynEntDefList[0] = AllocZeroed<DynEntityDef>(memory, count);
        clipMap.dynEntPoseList[0] = AllocZeroed<DynEntityPose>(memory, count);
        clipMap.dynEntClientList[0] = AllocZeroed<DynEntityClient>(memory, count);
        clipMap.dynEntCollList[0] = AllocZeroed<DynEntityColl>(memory, count);

        for (auto dynEntIndex = 0uz; dynEntIndex < count; dynEntIndex++)
        {
            const auto& entry = validDynModels[dynEntIndex];
            auto& dynEnt = clipMap.dynEntDefList[0][dynEntIndex];
            const auto origin = ParseFloat3(EntityField(*entry.block, "origin")).value_or(std::array<float, 3>{});
            const auto angles = ParseFloat3(EntityField(*entry.block, "angles")).value_or(std::array<float, 3>{});
            float axis[3][3]{};

            AnglesToAxis(angles, axis);

            dynEnt.type = DynEntityTypeFromString(EntityField(*entry.block, "type"));
            dynEnt.xModel = entry.model;
            dynEnt.physPreset = entry.physPreset;
            dynEnt.health = ParseInt(EntityField(*entry.block, "health"));
            dynEnt.contents = entry.model ? entry.model->contents : 0;
            std::copy(origin.begin(), origin.end(), dynEnt.pose.origin);
            AxisToQuat(axis, dynEnt.pose.quat);
            PopulateDynModelMass(*entry.block, dynEnt);
        }
    }

    [[nodiscard]] char PlaneTypeForNormal(const float (&normal)[3])
    {
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            if (normal[axis] != 1.0f)
                continue;

            auto isAxial = true;
            for (auto otherAxis = 0uz; otherAxis < 3uz; otherAxis++)
            {
                if (otherAxis != axis && normal[otherAxis] != 0.0f)
                    isAxial = false;
            }

            if (isAxial)
                return static_cast<char>(axis);
        }

        return 3;
    }

    [[nodiscard]] char PlaneSignBitsForNormal(const float (&normal)[3])
    {
        auto signBits = 0u;
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            if (normal[axis] < 0.0f)
                signBits |= 1u << axis;
        }

        return static_cast<char>(signBits);
    }

    [[nodiscard]] float RadiusFromBounds(const float (&mins)[3], const float (&maxs)[3])
    {
        auto radiusSquared = 0.0f;
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            const auto extent = std::max(std::abs(mins[axis]), std::abs(maxs[axis]));
            radiusSquared += extent * extent;
        }

        return std::sqrt(radiusSquared);
    }

    [[nodiscard]] int LeafTerrainContents(const clipMap_t& clipMap, const cLeaf_t& leaf)
    {
        auto contents = 0;
        if (!clipMap.aabbTrees || !clipMap.materials)
            return contents;

        const auto endIndex = std::min<unsigned>(clipMap.aabbTreeCount, static_cast<unsigned>(leaf.firstCollAabbIndex) + leaf.collAabbCount);
        for (auto aabbIndex = static_cast<unsigned>(leaf.firstCollAabbIndex); aabbIndex < endIndex; aabbIndex++)
        {
            const auto materialIndex = clipMap.aabbTrees[aabbIndex].materialIndex;
            if (materialIndex < clipMap.numMaterials)
                contents |= clipMap.materials[materialIndex].contentFlags;
        }

        return contents;
    }

    [[nodiscard]] int BrushContents(const clipMap_t& clipMap, const cbrush_t& brush)
    {
        auto contents = 0;
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            for (auto side = 0uz; side < 2uz; side++)
            {
                const auto materialIndex = brush.axialMaterialNum[side][axis];
                if (materialIndex >= 0 && static_cast<unsigned>(materialIndex) < clipMap.numMaterials)
                    contents |= clipMap.materials[materialIndex].contentFlags;
            }
        }

        for (auto sideIndex = 0uz; sideIndex < brush.numsides; sideIndex++)
        {
            const auto materialIndex = brush.sides[sideIndex].materialNum;
            if (materialIndex < clipMap.numMaterials)
                contents |= clipMap.materials[materialIndex].contentFlags;
        }

        return contents;
    }

    void InitLeafBrushNode(cLeafBrushNode_s& node)
    {
        node.axis = 0;
        node.leafBrushCount = 0;
        node.contents = 0;
        node.data.leaf.brushes = nullptr;
        node.data.children.dist = -std::numeric_limits<float>::max();
        node.data.children.range = 0.0f;
        node.data.children.childOffset[0] = 0u;
        node.data.children.childOffset[1] = 0u;
    }

    [[nodiscard]] size_t AllocLeafBrushNode(std::vector<cLeafBrushNode_s>& nodes)
    {
        const auto index = nodes.size();
        auto& node = nodes.emplace_back();
        node = {};
        InitLeafBrushNode(node);
        return index;
    }

    [[nodiscard]] float LeafBrushPartitionScore(
        const clipMap_t& clipMap,
        const LeafBrush* leafBrushes,
        const int leafBrushCount,
        const int axis,
        const float (&mins)[3],
        const float (&maxs)[3],
        float& dist)
    {
        auto rightBrushCount = -1;
        auto leftBrushCount = -1;
        auto min = -std::numeric_limits<float>::max();
        auto max = std::numeric_limits<float>::max();

        for (auto brushOffset = 0; brushOffset < leafBrushCount; brushOffset++)
        {
            const auto brushIndex = leafBrushes[brushOffset];
            const auto& brush = clipMap.brushes[brushIndex];
            if (dist > brush.mins[axis])
            {
                if (dist >= brush.maxs[axis])
                {
                    leftBrushCount++;
                    min = std::max(min, brush.maxs[axis]);
                }
            }
            else
            {
                rightBrushCount++;
                max = std::min(max, brush.mins[axis]);
            }
        }

        const auto scoreBrushCount = std::min(rightBrushCount, leftBrushCount);
        dist = (min + max) * 0.5f;
        if (scoreBrushCount <= 0)
            return 0.0f;

        return static_cast<float>(scoreBrushCount) * std::min(max - mins[axis], maxs[axis] - min);
    }

    [[nodiscard]] std::optional<size_t> PartitionLeafBrushes_r(
        const clipMap_t& clipMap,
        std::vector<cLeafBrushNode_s>& nodes,
        LeafBrush* leafBrushes,
        const int leafBrushCount,
        const float (&mins)[3],
        const float (&maxs)[3],
        std::string& error)
    {
        if (leafBrushCount <= 0)
        {
            error = "cannot partition an empty leafbrush range";
            return std::nullopt;
        }

        const auto nodeIndex = AllocLeafBrushNode(nodes);
        auto bestScore = 0.0f;
        auto axis = -1;
        auto dist = 0.0f;

        for (auto testAxis = 0; testAxis < 3; testAxis++)
        {
            for (auto brushOffset = 0; brushOffset < leafBrushCount; brushOffset++)
            {
                const auto brushIndex = leafBrushes[brushOffset];
                const auto& brush = clipMap.brushes[brushIndex];

                auto testDist = brush.mins[testAxis];
                auto score = LeafBrushPartitionScore(clipMap, leafBrushes, leafBrushCount, testAxis, mins, maxs, testDist);
                if (bestScore < score)
                {
                    bestScore = score;
                    axis = testAxis;
                    dist = testDist;
                }

                testDist = brush.maxs[testAxis];
                score = LeafBrushPartitionScore(clipMap, leafBrushes, leafBrushCount, testAxis, mins, maxs, testDist);
                if (bestScore < score)
                {
                    bestScore = score;
                    axis = testAxis;
                    dist = testDist;
                }
            }
        }

        if (axis >= 0)
        {
            std::vector<LeafBrush> leafBrushesCopy(leafBrushes, leafBrushes + leafBrushCount);
            auto* childLeafBrushes = leafBrushes;
            auto centerBrushCount = 0;
            for (const auto brushIndex : leafBrushesCopy)
            {
                const auto& brush = clipMap.brushes[brushIndex];
                if (dist > brush.mins[axis] && dist < brush.maxs[axis])
                    childLeafBrushes[centerBrushCount++] = brushIndex;
            }

            if (centerBrushCount > 0)
            {
                const auto childNodeIndex = PartitionLeafBrushes_r(clipMap, nodes, childLeafBrushes, centerBrushCount, mins, maxs, error);
                if (!childNodeIndex)
                    return std::nullopt;

                nodes[nodeIndex].leafBrushCount = -1;
                nodes[nodeIndex].contents = nodes[*childNodeIndex].contents;
                childLeafBrushes += centerBrushCount;
            }

            auto range = std::numeric_limits<float>::max();
            nodes[nodeIndex].axis = static_cast<char>(axis);
            nodes[nodeIndex].data.children.dist = dist;

            for (auto side = 0; side < 2; side++)
            {
                auto childBrushCount = 0;
                for (const auto brushIndex : leafBrushesCopy)
                {
                    const auto& brush = clipMap.brushes[brushIndex];
                    if (side != 0)
                    {
                        if (dist < brush.maxs[axis])
                            continue;

                        range = std::min(range, dist - brush.maxs[axis]);
                    }
                    else
                    {
                        if (dist > brush.mins[axis])
                            continue;

                        range = std::min(range, brush.mins[axis] - dist);
                    }

                    childLeafBrushes[childBrushCount++] = brushIndex;
                }

                if (childBrushCount <= 0)
                {
                    error = "leafbrush partition produced an empty child";
                    return std::nullopt;
                }

                float childMins[3]{mins[0], mins[1], mins[2]};
                float childMaxs[3]{maxs[0], maxs[1], maxs[2]};
                if (side != 0)
                    childMaxs[axis] = dist - range;
                else
                    childMins[axis] = dist + range;

                const auto childNodeIndex = PartitionLeafBrushes_r(clipMap, nodes, childLeafBrushes, childBrushCount, childMins, childMaxs, error);
                if (!childNodeIndex)
                    return std::nullopt;

                const auto childOffset = *childNodeIndex - nodeIndex;
                if (childOffset > std::numeric_limits<uint16_t>::max())
                {
                    error = "leafbrush partition child offset exceeded uint16 range";
                    return std::nullopt;
                }

                nodes[nodeIndex].data.children.childOffset[side] = static_cast<uint16_t>(childOffset);
                nodes[nodeIndex].contents |= nodes[*childNodeIndex].contents;
                childLeafBrushes += childBrushCount;
            }

            nodes[nodeIndex].data.children.range = range;
            return nodeIndex;
        }

        if (leafBrushCount > std::numeric_limits<int16_t>::max())
        {
            error = "leafbrush partition leaf count exceeded int16 range";
            return std::nullopt;
        }

        nodes[nodeIndex].leafBrushCount = static_cast<int16_t>(leafBrushCount);
        for (auto brushOffset = 0; brushOffset < leafBrushCount; brushOffset++)
        {
            const auto brushIndex = leafBrushes[brushOffset];
            nodes[nodeIndex].contents |= clipMap.brushes[brushIndex].contents;
        }

        if (nodes[nodeIndex].contents == 0)
        {
            error = "leafbrush partition produced a leaf with no contents";
            return std::nullopt;
        }

        nodes[nodeIndex].data.leaf.brushes = leafBrushes;
        return nodeIndex;
    }

    [[nodiscard]] bool PartitionLeafBrushes(
        const clipMap_t& clipMap,
        std::vector<cLeafBrushNode_s>& nodes,
        LeafBrush* leafBrushes,
        const int leafBrushCount,
        cLeaf_t& leaf,
        std::string& error)
    {
        leaf.brushContents = 0;
        leaf.terrainContents = LeafTerrainContents(clipMap, leaf);
        leaf.leafBrushNode = 0;

        if (leafBrushCount <= 0)
            return true;

        float mins[3]{
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
        };
        float maxs[3]{
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
        };

        for (auto brushOffset = 0; brushOffset < leafBrushCount; brushOffset++)
        {
            const auto brushIndex = leafBrushes[brushOffset];
            if (brushIndex >= clipMap.numBrushes)
            {
                error = "leafbrush references invalid brush";
                return false;
            }

            const auto& brush = clipMap.brushes[brushIndex];
            leaf.brushContents |= brush.contents;
            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                mins[axis] = std::min(mins[axis], brush.mins[axis]);
                maxs[axis] = std::max(maxs[axis], brush.maxs[axis]);
            }
        }

        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            leaf.mins[axis] = mins[axis] - 0.125f;
            leaf.maxs[axis] = maxs[axis] + 0.125f;
        }

        const auto nodeIndex = PartitionLeafBrushes_r(clipMap, nodes, leafBrushes, leafBrushCount, mins, maxs, error);
        if (!nodeIndex)
            return false;

        leaf.leafBrushNode = static_cast<int>(*nodeIndex);
        return true;
    }

    [[nodiscard]] bool PopulateClipMapMaterials(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* materials = bsp.GetLump(LUMP_MATERIALS);
        if (!ValidateRecordLump(bsp, materials, LUMP_MATERIALS, RAW_MATERIAL_SIZE, error))
            return false;

        const auto count = RecordCount(*materials, RAW_MATERIAL_SIZE);
        if (!FitsUnsigned(count))
        {
            error = "too many material records";
            return false;
        }

        clipMap.numMaterials = static_cast<unsigned>(count);
        clipMap.materials = AllocZeroed<dmaterial_t>(memory, count);

        for (auto i = 0uz; i < count; i++)
        {
            const auto* record = materials->data.data() + i * RAW_MATERIAL_SIZE;
            std::memcpy(clipMap.materials[i].material, record, sizeof(clipMap.materials[i].material));
            clipMap.materials[i].surfaceFlags = ReadI32(record, 64uz);
            // linker_pc strips raw-only BSP content bits before storing the
            // runtime clipMap material table. Brush contents are derived from
            // this masked value, while the dumper reconstructs the raw marker
            // where needed when writing a .d3dbsp back out.
            clipMap.materials[i].contentFlags =
                static_cast<int>(static_cast<unsigned>(ReadI32(record, 68uz)) & IW3::d3dbsp::RUNTIME_MATERIAL_CONTENT_MASK);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapPlanes(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* planes = bsp.GetLump(LUMP_PLANES);
        if (!ValidateRecordLump(bsp, planes, LUMP_PLANES, RAW_PLANE_SIZE, error))
            return false;

        const auto count = RecordCount(*planes, RAW_PLANE_SIZE);
        if (!FitsInt(count))
        {
            error = "too many plane records";
            return false;
        }

        clipMap.planeCount = static_cast<int>(count);
        clipMap.planes = AllocZeroed<cplane_s>(memory, count);

        for (auto i = 0uz; i < count; i++)
        {
            const auto* record = planes->data.data() + i * RAW_PLANE_SIZE;
            CopyFloat3(record, clipMap.planes[i].normal);
            clipMap.planes[i].dist = ReadFloat(record, 12uz);
            clipMap.planes[i].type = PlaneTypeForNormal(clipMap.planes[i].normal);
            clipMap.planes[i].signbits = PlaneSignBitsForNormal(clipMap.planes[i].normal);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapBrushes(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* brushHeaders = bsp.GetLump(LUMP_BRUSHES);
        const auto* brushSides = bsp.GetLump(LUMP_BRUSHSIDES);
        const auto* edgeCounts = bsp.GetLump(LUMP_BRUSHSIDE_EDGE_COUNTS);
        const auto* brushEdges = bsp.GetLump(LUMP_BRUSHEDGES);

        if (!ValidateRecordLump(bsp, brushHeaders, LUMP_BRUSHES, RAW_BRUSH_HEADER_SIZE, error)
            || !ValidateRecordLump(bsp, brushSides, LUMP_BRUSHSIDES, RAW_BRUSHSIDE_SIZE, error) || !edgeCounts)
            return false;

        const auto brushCount = RecordCount(*brushHeaders, RAW_BRUSH_HEADER_SIZE);
        if (!FitsUint16(brushCount))
        {
            error = "too many brush records";
            return false;
        }

        auto totalSideCount = 0uz;
        auto nonAxialSideCount = 0uz;
        for (auto brushIndex = 0uz; brushIndex < brushCount; brushIndex++)
        {
            const auto sideCount = ReadU16(brushHeaders->data.data() + brushIndex * RAW_BRUSH_HEADER_SIZE);
            if (sideCount < 6u)
            {
                error = "brush has fewer than six axial sides";
                return false;
            }

            totalSideCount += sideCount;
            nonAxialSideCount += sideCount - 6u;
        }

        if (brushSides->data.size() != totalSideCount * RAW_BRUSHSIDE_SIZE || edgeCounts->data.size() != totalSideCount)
        {
            error = "brush side/edge-count lumps do not match brush headers";
            return false;
        }

        if (!FitsUnsigned(nonAxialSideCount))
        {
            error = "too many non-axial brush sides";
            return false;
        }

        clipMap.numBrushes = static_cast<uint16_t>(brushCount);
        // The stock linker allocates one extra brush after the raw brush array
        // for the runtime box hull used by CM_InitThreadData/trace code. Keep
        // numBrushes as the raw count; box_brush points at the extra slot.
        clipMap.brushes = AllocZeroed<cbrush_t>(memory, brushCount + 1uz);
        clipMap.numBrushSides = static_cast<unsigned>(nonAxialSideCount);
        clipMap.brushsides = nonAxialSideCount > 0uz ? AllocZeroed<cbrushside_t>(memory, nonAxialSideCount) : nullptr;
        clipMap.numBrushEdges = brushEdges ? static_cast<unsigned>(brushEdges->data.size()) : 0u;
        clipMap.brushEdges = brushEdges && !brushEdges->data.empty() ? AllocCopy<cbrushedge_t>(memory, brushEdges->data) : nullptr;

        auto rawSideIndex = 0uz;
        auto nonAxialSideIndex = 0uz;
        auto edgeOffset = 0uz;

        for (auto brushIndex = 0uz; brushIndex < brushCount; brushIndex++)
        {
            auto& brush = clipMap.brushes[brushIndex];
            const auto* header = brushHeaders->data.data() + brushIndex * RAW_BRUSH_HEADER_SIZE;
            const auto sideCount = ReadU16(header);
            const auto nonAxialCount = static_cast<unsigned>(sideCount - 6u);

            brush.numsides = nonAxialCount;
            brush.sides = nonAxialCount > 0u ? &clipMap.brushsides[nonAxialSideIndex] : nullptr;
            brush.baseAdjacentSide = clipMap.brushEdges && edgeOffset < clipMap.numBrushEdges ? &clipMap.brushEdges[edgeOffset] : nullptr;

            auto localEdgeOffset = 0uz;
            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                for (auto side = 0uz; side < 2uz; side++)
                {
                    const auto* rawSide = brushSides->data.data() + rawSideIndex * RAW_BRUSHSIDE_SIZE;
                    const auto materialIndex = static_cast<int16_t>(ReadU32(rawSide, 4uz));
                    const auto edgeCount = std::to_integer<unsigned char>(edgeCounts->data[rawSideIndex]);

                    if (side == 0uz)
                        brush.mins[axis] = ReadFloat(rawSide);
                    else
                        brush.maxs[axis] = ReadFloat(rawSide);

                    brush.axialMaterialNum[side][axis] = materialIndex;
                    brush.firstAdjacentSideOffsets[side][axis] = static_cast<int16_t>(localEdgeOffset);
                    brush.edgeCount[side][axis] = static_cast<char>(edgeCount);
                    localEdgeOffset += edgeCount;
                    rawSideIndex++;
                }
            }

            for (auto sideIndex = 0uz; sideIndex < nonAxialCount; sideIndex++)
            {
                const auto* rawSide = brushSides->data.data() + rawSideIndex * RAW_BRUSHSIDE_SIZE;
                const auto planeIndex = ReadU32(rawSide);
                const auto materialIndex = ReadU32(rawSide, 4uz);
                if (planeIndex >= static_cast<unsigned>(std::max(clipMap.planeCount, 0)))
                {
                    error = "brush side references an invalid plane";
                    return false;
                }

                auto& side = brush.sides[sideIndex];
                side.plane = &clipMap.planes[planeIndex];
                side.materialNum = materialIndex;
                side.firstAdjacentSideOffset = static_cast<int16_t>(localEdgeOffset);
                side.edgeCount = static_cast<char>(std::to_integer<unsigned char>(edgeCounts->data[rawSideIndex]));
                localEdgeOffset += static_cast<unsigned char>(side.edgeCount);
                rawSideIndex++;
            }

            edgeOffset += localEdgeOffset;
            nonAxialSideIndex += nonAxialCount;
            brush.contents = BrushContents(clipMap, brush);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapNodes(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* nodes = bsp.GetLump(LUMP_NODES);
        if (!nodes)
            return true;

        if (nodes->data.size() % RAW_CLIP_NODE_SIZE != 0uz)
        {
            error = "node lump has funny size";
            return false;
        }

        const auto nodeCount = RecordCount(*nodes, RAW_CLIP_NODE_SIZE);
        if (!FitsUnsigned(nodeCount))
        {
            error = "too many node records";
            return false;
        }

        clipMap.numNodes = static_cast<unsigned>(nodeCount);
        clipMap.nodes = AllocZeroed<cNode_t>(memory, nodeCount);

        for (auto nodeIndex = 0uz; nodeIndex < nodeCount; nodeIndex++)
        {
            const auto* record = nodes->data.data() + nodeIndex * RAW_CLIP_NODE_SIZE;
            const auto planeIndex = ReadI32(record);
            const auto child0 = ReadI32(record, 4uz);
            const auto child1 = ReadI32(record, 8uz);

            if (planeIndex < 0 || planeIndex >= clipMap.planeCount || !FitsInt16(child0) || !FitsInt16(child1))
            {
                error = "node record references invalid plane or child";
                return false;
            }

            clipMap.nodes[nodeIndex].plane = &clipMap.planes[planeIndex];
            clipMap.nodes[nodeIndex].children[0] = static_cast<int16_t>(child0);
            clipMap.nodes[nodeIndex].children[1] = static_cast<int16_t>(child1);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapLeafBrushes(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* leafBrushes = bsp.GetLump(LUMP_LEAFBRUSHES);
        if (!leafBrushes)
        {
            clipMap.numLeafBrushes = 0u;
            clipMap.leafbrushes = AllocZeroed<LeafBrush>(memory, 1uz);
            return true;
        }

        if (leafBrushes->data.size() % RAW_LEAF_BRUSH_SIZE != 0uz)
        {
            error = "leafbrush lump has funny size";
            return false;
        }

        const auto leafBrushCount = RecordCount(*leafBrushes, RAW_LEAF_BRUSH_SIZE);
        if (!FitsUnsigned(leafBrushCount))
        {
            error = "too many leafbrush records";
            return false;
        }

        clipMap.numLeafBrushes = static_cast<unsigned>(leafBrushCount);
        // CM_InitBoxHull writes one extra entry at leafbrushes[numLeafBrushes]
        // without increasing numLeafBrushes. Allocate that spare slot so the
        // serialized box leaf node can reference it safely.
        clipMap.leafbrushes = AllocZeroed<LeafBrush>(memory, leafBrushCount + 1uz);

        for (auto i = 0uz; i < leafBrushCount; i++)
        {
            const auto brushIndex = ReadU32(leafBrushes->data.data() + i * RAW_LEAF_BRUSH_SIZE);
            if (brushIndex > std::numeric_limits<LeafBrush>::max())
            {
                error = "leafbrush index exceeds runtime range";
                return false;
            }

            clipMap.leafbrushes[i] = static_cast<LeafBrush>(brushIndex);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapCollision(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* verts = bsp.GetLump(LUMP_COLLISIONVERTS);
        if (verts)
        {
            if (verts->data.size() % RAW_VEC3_SIZE != 0uz || !FitsUnsigned(RecordCount(*verts, RAW_VEC3_SIZE)))
            {
                error = "collision vert lump has funny size";
                return false;
            }

            clipMap.vertCount = static_cast<unsigned>(RecordCount(*verts, RAW_VEC3_SIZE));
            clipMap.verts = AllocCopy<vec3_t>(memory, verts->data);
        }

        const auto* tris = bsp.GetLump(LUMP_COLLISIONTRIS);
        if (tris)
        {
            if (tris->data.size() % RAW_TRI_INDICES_SIZE != 0uz || !FitsInt(RecordCount(*tris, RAW_TRI_INDICES_SIZE)))
            {
                error = "collision tri lump has funny size";
                return false;
            }

            clipMap.triCount = static_cast<int>(RecordCount(*tris, RAW_TRI_INDICES_SIZE));
            clipMap.triIndices = AllocCopy<uint16_t>(memory, tris->data);
        }

        const auto* walkable = bsp.GetLump(LUMP_COLLISION_EDGE_WALKABLE);
        if (walkable)
            clipMap.triEdgeIsWalkable = AllocCopy<char>(memory, walkable->data);

        const auto* borders = bsp.GetLump(LUMP_COLLISIONBORDERS);
        if (borders)
        {
            if (borders->data.size() % RAW_COLLISION_BORDER_SIZE != 0uz || !FitsInt(RecordCount(*borders, RAW_COLLISION_BORDER_SIZE)))
            {
                error = "collision border lump has funny size";
                return false;
            }

            clipMap.borderCount = static_cast<int>(RecordCount(*borders, RAW_COLLISION_BORDER_SIZE));
            clipMap.borders = AllocCopy<CollisionBorder>(memory, borders->data);
        }

        const auto* partitions = bsp.GetLump(LUMP_COLLISIONPARTITIONS);
        if (partitions)
        {
            if (partitions->data.size() % RAW_COLLISION_PARTITION_SIZE != 0uz || !FitsInt(RecordCount(*partitions, RAW_COLLISION_PARTITION_SIZE)))
            {
                error = "collision partition lump has funny size";
                return false;
            }

            const auto partitionCount = RecordCount(*partitions, RAW_COLLISION_PARTITION_SIZE);
            clipMap.partitionCount = static_cast<int>(partitionCount);
            clipMap.partitions = AllocZeroed<CollisionPartition>(memory, partitionCount);
            const auto runtimeBorderCount = static_cast<uint32_t>(std::max(clipMap.borderCount, 0));
            for (auto i = 0uz; i < partitionCount; i++)
            {
                const auto* record = partitions->data.data() + i * RAW_COLLISION_PARTITION_SIZE;
                const auto borderIndex = ReadU32(record, 8uz);
                const auto borderCount = static_cast<unsigned char>(std::to_integer<unsigned char>(record[3]));
                if (borderCount > 0u && (borderIndex > runtimeBorderCount || borderCount > runtimeBorderCount - borderIndex))
                {
                    error = "collision partition references invalid border";
                    return false;
                }

                clipMap.partitions[i].triCount = static_cast<unsigned char>(std::to_integer<unsigned char>(record[2]));
                clipMap.partitions[i].borderCount = borderCount;
                clipMap.partitions[i].firstTri = ReadI32(record, 4uz);
                // Raw partitions can retain an in-range border index even when
                // borderCount is zero. Runtime traces ignore it in that case,
                // but preserving the pointer lets the dumper reproduce the raw
                // index after an OAT link/unlink round trip.
                clipMap.partitions[i].borders = clipMap.borders && borderIndex < runtimeBorderCount ? &clipMap.borders[borderIndex] : nullptr;
            }
        }

        const auto* aabbs = bsp.GetLump(LUMP_COLLISIONAABBS);
        if (aabbs)
        {
            if (aabbs->data.size() % RAW_COLLISION_AABB_SIZE != 0uz || !FitsInt(RecordCount(*aabbs, RAW_COLLISION_AABB_SIZE)))
            {
                error = "collision AABB lump has funny size";
                return false;
            }

            clipMap.aabbTreeCount = static_cast<int>(RecordCount(*aabbs, RAW_COLLISION_AABB_SIZE));
            clipMap.aabbTrees = AllocCopy<CollisionAabbTree>(memory, aabbs->data);
        }

        return true;
    }

    [[nodiscard]] bool PopulateClipMapLeafs(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* leafs = bsp.GetLump(LUMP_LEAFS);
        if (!leafs)
            return true;

        if (leafs->data.size() % RAW_LEAF_SIZE != 0uz || !FitsUnsigned(RecordCount(*leafs, RAW_LEAF_SIZE)))
        {
            error = "leaf lump has funny size";
            return false;
        }

        const auto leafCount = RecordCount(*leafs, RAW_LEAF_SIZE);
        clipMap.numLeafs = static_cast<unsigned>(leafCount);
        clipMap.leafs = AllocZeroed<cLeaf_t>(memory, leafCount);

        auto maxCluster = -1;
        for (auto leafIndex = 0uz; leafIndex < leafCount; leafIndex++)
        {
            const auto* record = leafs->data.data() + leafIndex * RAW_LEAF_SIZE;
            auto& leaf = clipMap.leafs[leafIndex];
            const auto cluster = ReadI32(record);
            const auto firstCollAabbIndex = ReadI32(record, 4uz);
            const auto collAabbCount = ReadI32(record, 8uz);

            if (firstCollAabbIndex < 0 || collAabbCount < 0)
            {
                error = "leaf contains negative runtime count/index";
                return false;
            }

            leaf.firstCollAabbIndex = static_cast<uint16_t>(std::min(firstCollAabbIndex, static_cast<int>(std::numeric_limits<uint16_t>::max())));
            leaf.collAabbCount = static_cast<uint16_t>(std::min(collAabbCount, static_cast<int>(std::numeric_limits<uint16_t>::max())));
            leaf.cluster = static_cast<int16_t>(
                std::clamp(cluster, static_cast<int>(std::numeric_limits<int16_t>::min()), static_cast<int>(std::numeric_limits<int16_t>::max())));
            leaf.leafBrushNode = 0;

            if (cluster >= 0)
                maxCluster = std::max(maxCluster, cluster);
        }

        clipMap.numClusters = maxCluster + 1;
        return true;
    }

    [[nodiscard]] bool PopulateClipMapLeafBrushNodes(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* leafs = bsp.GetLump(LUMP_LEAFS);
        if (!leafs || !clipMap.leafs)
            return true;

        if (leafs->data.size() % RAW_LEAF_SIZE != 0uz || RecordCount(*leafs, RAW_LEAF_SIZE) != clipMap.numLeafs)
        {
            error = "leaf lump changed before leafbrush node build";
            return false;
        }

        // Stock cm_load_obj builds this array in temp memory with index 0 left
        // as an unused sentinel. Leaf traces assert leafBrushNode is non-zero,
        // so real nodes start at index 1.
        std::vector<cLeafBrushNode_s> nodes;
        nodes.reserve(static_cast<size_t>(clipMap.numLeafBrushes) + static_cast<size_t>(clipMap.numBrushes) + 2uz);
        nodes.emplace_back();
        nodes[0] = {};

        for (auto leafIndex = 0uz; leafIndex < clipMap.numLeafs; leafIndex++)
        {
            const auto* record = leafs->data.data() + leafIndex * RAW_LEAF_SIZE;
            const auto firstLeafBrush = ReadI32(record, 12uz);
            const auto leafBrushCount = ReadI32(record, 16uz);
            if (firstLeafBrush < 0 || leafBrushCount < 0)
            {
                error = "leaf contains negative leafbrush range";
                return false;
            }

            if (static_cast<size_t>(firstLeafBrush + leafBrushCount) > clipMap.numLeafBrushes)
            {
                error = "leaf references invalid leafbrush range";
                return false;
            }

            if (!PartitionLeafBrushes(clipMap, nodes, &clipMap.leafbrushes[firstLeafBrush], leafBrushCount, clipMap.leafs[leafIndex], error))
                return false;
        }

        const auto* models = bsp.GetLump(LUMP_MODELS);
        if (models && clipMap.cmodels)
        {
            if (models->data.size() % RAW_MODEL_SIZE != 0uz || RecordCount(*models, RAW_MODEL_SIZE) != clipMap.numSubModels)
            {
                error = "model lump changed before leafbrush node build";
                return false;
            }

            for (auto modelIndex = 1uz; modelIndex < clipMap.numSubModels; modelIndex++)
            {
                const auto* record = models->data.data() + modelIndex * RAW_MODEL_SIZE;
                const auto firstBrush = ReadU32(record, 40uz);
                const auto brushCount = ReadU32(record, 44uz);
                if (brushCount == 0u)
                    continue;

                if (firstBrush + brushCount > clipMap.numBrushes)
                {
                    error = "model references invalid brush range";
                    return false;
                }

                auto* modelLeafBrushes = AllocZeroed<LeafBrush>(memory, brushCount);
                for (auto brushOffset = 0u; brushOffset < brushCount; brushOffset++)
                    modelLeafBrushes[brushOffset] = static_cast<LeafBrush>(firstBrush + brushOffset);

                if (!PartitionLeafBrushes(clipMap, nodes, modelLeafBrushes, static_cast<int>(brushCount), clipMap.cmodels[modelIndex].leaf, error))
                    return false;
            }
        }

        clipMap.box_brush = &clipMap.brushes[clipMap.numBrushes];
        clipMap.box_brush->contents = -1;
        clipMap.box_brush->sides = nullptr;
        clipMap.box_brush->baseAdjacentSide = nullptr;
        for (auto side = 0uz; side < 2uz; side++)
        {
            for (auto axis = 0uz; axis < 3uz; axis++)
                clipMap.box_brush->axialMaterialNum[side][axis] = -1;
        }

        clipMap.box_model.leaf.brushContents = -1;
        clipMap.box_model.leaf.terrainContents = 0;
        clipMap.box_model.leaf.leafBrushNode = static_cast<int>(AllocLeafBrushNode(nodes));
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            clipMap.box_model.leaf.mins[axis] = std::numeric_limits<float>::max();
            clipMap.box_model.leaf.maxs[axis] = -std::numeric_limits<float>::max();
        }

        auto& boxLeafBrushNode = nodes[clipMap.box_model.leaf.leafBrushNode];
        boxLeafBrushNode.leafBrushCount = 1;
        boxLeafBrushNode.data.leaf.brushes = &clipMap.leafbrushes[clipMap.numLeafBrushes];
        clipMap.leafbrushes[clipMap.numLeafBrushes] = clipMap.numBrushes;

        clipMap.leafbrushNodesCount = static_cast<unsigned>(nodes.size());
        clipMap.leafbrushNodes = AllocZeroed<cLeafBrushNode_s>(memory, nodes.size());
        std::copy(nodes.begin(), nodes.end(), clipMap.leafbrushNodes);
        return true;
    }

    [[nodiscard]] bool PopulateClipMapModels(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* models = bsp.GetLump(LUMP_MODELS);
        if (!models)
            return true;

        if (models->data.size() % RAW_MODEL_SIZE != 0uz || !FitsUnsigned(RecordCount(*models, RAW_MODEL_SIZE)))
        {
            error = "model lump has funny size";
            return false;
        }

        const auto modelCount = RecordCount(*models, RAW_MODEL_SIZE);
        clipMap.numSubModels = static_cast<unsigned>(modelCount);
        clipMap.cmodels = AllocZeroed<cmodel_t>(memory, modelCount);

        for (auto modelIndex = 0uz; modelIndex < modelCount; modelIndex++)
        {
            const auto* record = models->data.data() + modelIndex * RAW_MODEL_SIZE;
            auto& model = clipMap.cmodels[modelIndex];
            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                model.mins[axis] = ReadFloat(record, axis * sizeof(float)) - 1.0f;
                model.maxs[axis] = ReadFloat(record, 12uz + axis * sizeof(float)) + 1.0f;
            }
            model.radius = RadiusFromBounds(model.mins, model.maxs);

            if (modelIndex > 0uz)
            {
                const auto firstCollAabbIndex = ReadU32(record, 32uz);
                const auto collAabbCount = ReadU32(record, 36uz);
                if (firstCollAabbIndex > std::numeric_limits<uint16_t>::max() || collAabbCount > std::numeric_limits<uint16_t>::max())
                {
                    error = "model collision AABB range exceeded uint16";
                    return false;
                }

                model.leaf.firstCollAabbIndex = static_cast<uint16_t>(firstCollAabbIndex);
                model.leaf.collAabbCount = static_cast<uint16_t>(collAabbCount);
            }
        }

        return PopulateClipMapLeafBrushNodes(clipMap, bsp, memory, error);
    }

    [[nodiscard]] bool PopulateClipMapVisibility(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* visibility = bsp.GetLump(LUMP_VISIBILITY);
        if (!visibility || visibility->data.empty())
        {
            // CMod_LoadVisibility synthesizes an all-visible table when the BSP
            // has no visibility lump. It sizes clusterBytes from the leaf
            // cluster count collected earlier, then collapses numClusters to 1.
            clipMap.clusterBytes = ((clipMap.numClusters + 63) & ~63) >> 3;
            clipMap.numClusters = 1;
            clipMap.visibility = AllocZeroed<char>(memory, static_cast<size_t>(std::max(clipMap.clusterBytes, 0)));
            FillArray(clipMap.visibility, static_cast<size_t>(std::max(clipMap.clusterBytes, 0)), 0xffu);
            return true;
        }

        if (visibility->data.size() < 8uz)
        {
            error = "visibility lump has a truncated header";
            return false;
        }

        const auto numClusters = ReadI32(visibility->data.data());
        const auto clusterBytes = ReadI32(visibility->data.data(), 4uz);
        if (numClusters < 0 || clusterBytes < 0)
        {
            error = "visibility lump has negative dimensions";
            return false;
        }

        const auto expectedSize = 8uz + static_cast<size_t>(numClusters) * static_cast<size_t>(clusterBytes);
        if (visibility->data.size() != expectedSize)
        {
            error = "visibility lump size does not match its header";
            return false;
        }

        clipMap.numClusters = numClusters;
        clipMap.clusterBytes = clusterBytes;
        clipMap.visibility = AllocZeroed<char>(memory, visibility->data.size() - 8uz);
        if (clipMap.visibility)
            std::memcpy(clipMap.visibility, visibility->data.data() + 8uz, visibility->data.size() - 8uz);
        clipMap.vised = 1;
        return true;
    }

    [[nodiscard]] bool PopulateClipMapLeafSurfaces(clipMap_t& clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* leafSurfaces = bsp.GetLump(LUMP_LEAFSURFACES);
        if (!leafSurfaces)
            return true;

        if (leafSurfaces->data.size() % sizeof(uint32_t) != 0uz || !FitsUnsigned(leafSurfaces->data.size() / sizeof(uint32_t)))
        {
            error = "leafsurface lump has funny size";
            return false;
        }

        clipMap.numLeafSurfaces = static_cast<unsigned>(leafSurfaces->data.size() / sizeof(uint32_t));
        clipMap.leafsurfaces = AllocCopy<unsigned>(memory, leafSurfaces->data);
        return true;
    }

    [[nodiscard]] IW3::d3dbsp::TrisType ChooseTrisContextType(const IW3::d3dbsp::File& bsp)
    {
        // Stock R_ChooseTrisContextType selects the unlayered path when that
        // geometry exists and layered materials are disabled. OAT currently
        // mirrors that linker_pc-compatible path.
        const auto* unlayeredSurfaces = bsp.GetLump(LUMP_SIMPLE_TRI_SOUPS);
        return unlayeredSurfaces && !unlayeredSurfaces->data.empty() ? TRIS_TYPE_SIMPLE : TRIS_TYPE_LAYERED;
    }

    [[nodiscard]] const IW3::d3dbsp::Lump* SelectWorldLumpForTrisType(
        const IW3::d3dbsp::File& bsp,
        const IW3::d3dbsp::LumpType layered,
        const IW3::d3dbsp::LumpType unlayered)
    {
        return bsp.GetLump(ChooseTrisContextType(bsp) == TRIS_TYPE_SIMPLE ? unlayered : layered);
    }

    [[nodiscard]] bool UsesSimpleWorldGeometry(const IW3::d3dbsp::File& bsp)
    {
        return ChooseTrisContextType(bsp) == TRIS_TYPE_SIMPLE;
    }

    struct LightmapAtlasGroup
    {
        unsigned wideCount = 1u;
        unsigned highCount = 1u;
        std::vector<unsigned> rawPageForPackedSlot;
    };

    struct LightmapAtlasLayout
    {
        unsigned rawPageCount = 0u;
        std::vector<LightmapAtlasGroup> groups;
        std::vector<unsigned> atlasIndexForRawPage;
        std::vector<unsigned> packedSlotForRawPage;
    };

    [[nodiscard]] bool CopyLightDefAttenuationImage(
        std::vector<std::byte>& secondary,
        const LightmapAtlasGroup& group,
        const GfxLightDef& lightDef,
        ISearchPath& searchPath,
        std::string& error)
    {
        if (lightDef.lmapLookupStart <= 0)
        {
            error = std::format("light def \"{}\" has invalid lightmap lookup start", lightDef.name ? lightDef.name : "");
            return false;
        }

        std::vector<RawLightPixel> pixels;
        if (!LoadLightDefAttenuationPixels(lightDef, searchPath, pixels, error))
            return false;

        if (pixels.empty())
            return true;

        const auto zoom = group.wideCount;
        const auto firstPixelOffset = static_cast<size_t>(zoom) * static_cast<size_t>(lightDef.lmapLookupStart - 1);
        size_t pixelOffset = firstPixelOffset;

        const auto writePixel = [&](const RawLightPixel& pixel)
        {
            if ((pixelOffset + 1uz) * LIGHTMAP_SECONDARY_PIXEL_SIZE > secondary.size())
                return false;

            WriteSecondaryLightmapPixel(secondary, pixelOffset, pixel);
            pixelOffset++;
            return true;
        };

        if (zoom == 1u)
        {
            if (!writePixel(pixels.front()))
                return false;

            for (const auto& pixel : pixels)
            {
                if (!writePixel(pixel))
                    return false;
            }

            if (!writePixel(pixels.back()))
                return false;
        }
        else
        {
            if ((zoom & (zoom - 1u)) != 0u)
            {
                error = "lightmap atlas zoom is not a power of two";
                return false;
            }

            const auto endCount = zoom + (zoom >> 1u);
            for (auto i = 0u; i < endCount; i++)
            {
                if (!writePixel(pixels.front()))
                    return false;
            }

            for (auto pixelIndex = 0uz; pixelIndex + 1uz < pixels.size(); pixelIndex++)
            {
                for (auto lerp = 1u; lerp <= 2u * zoom; lerp += 2u)
                {
                    if (!writePixel(LerpLightPixel(pixels[pixelIndex], pixels[pixelIndex + 1uz], zoom, lerp)))
                        return false;
                }
            }

            for (auto i = 0u; i < endCount; i++)
            {
                if (!writePixel(pixels.back()))
                    return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool ApplyLightDefAttenuationImages(
        std::vector<std::byte>& secondary,
        const LightmapAtlasGroup& group,
        const std::vector<GfxLightDef*>& lightDefs,
        ISearchPath& searchPath,
        std::string& error)
    {
        // linker_pc overlays loaded lightdef falloff images into each generated
        // secondary lightmap atlas. These bytes are not authored in the raw BSP
        // LIGHTMAPS lump, but they are present after link -> unlink canonicalizes it.
        for (const auto* lightDef : lightDefs)
        {
            if (!lightDef)
                continue;

            if (!CopyLightDefAttenuationImage(secondary, group, *lightDef, searchPath, error))
            {
                if (error.empty())
                    error = std::format("light def \"{}\" attenuation image overflowed the secondary lightmap atlas", lightDef->name ? lightDef->name : "");
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] std::vector<GfxLightDef*> LoadPrimaryLightDefDependencies(
        const IW3::d3dbsp::File& bsp,
        AssetCreationContext& context,
        AssetRegistration<AssetGfxWorld>& registration,
        std::string& error)
    {
        std::vector<GfxLightDef*> lightDefs;
        const auto* primaryLights = bsp.GetLump(LUMP_PRIMARY_LIGHTS);
        if (!primaryLights || primaryLights->data.empty())
            return lightDefs;

        if (primaryLights->data.size() % IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE != 0uz)
        {
            error = "primary-light lump has funny size";
            return lightDefs;
        }

        std::unordered_map<std::string, bool> loadedLightDefs;
        const auto primaryLightCount = RecordCount(*primaryLights, IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE);
        for (auto lightIndex = 0uz; lightIndex < primaryLightCount; lightIndex++)
        {
            const auto* record = primaryLights->data.data() + lightIndex * IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE;
            const auto defName = RawString(record + RAW_LIGHT_DEF_NAME_OFFSET, RAW_LIGHT_DEF_NAME_SIZE);
            if (defName.empty())
                continue;

            const auto lookupName = LowercaseAscii(defName);
            if (loadedLightDefs.find(lookupName) != loadedLightDefs.end())
                continue;

            loadedLightDefs.emplace(lookupName, true);
            auto* dependency = context.LoadDependency<AssetLightDef>(defName);
            if (!dependency)
            {
                error = std::format("missing light def \"{}\"", defName);
                return lightDefs;
            }

            auto* lightDef = dependency->Asset();
            if (!lightDef || !lightDef->attenuation.image)
            {
                error = std::format("light def \"{}\" has no attenuation image", defName);
                return lightDefs;
            }

            registration.AddDependency(dependency);
            lightDefs.emplace_back(lightDef);
        }

        return lightDefs;
    }

    [[nodiscard]] int SaturatingAdd(const int left, const int right)
    {
        if (right > 0 && left > std::numeric_limits<int>::max() - right)
            return std::numeric_limits<int>::max();

        return left + right;
    }

    [[nodiscard]] bool BuildLightmapCouplingMatrix(
        const IW3::d3dbsp::File& bsp,
        const unsigned rawPageCount,
        std::array<int, MAX_LIGHTMAP_PAGE_COUNT * MAX_LIGHTMAP_PAGE_COUNT>& coupling,
        std::string& error)
    {
        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        if (!surfaces)
            return true;

        if (surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz)
        {
            error = "world surface lump has funny size";
            return false;
        }

        const auto* materials = bsp.GetLump(LUMP_MATERIALS);
        const auto materialCount = materials && materials->data.size() % RAW_MATERIAL_SIZE == 0uz ? RecordCount(*materials, RAW_MATERIAL_SIZE) : 0uz;
        const auto surfaceCount = RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE);

        for (auto materialIndex = 0uz; materialIndex < materialCount; materialIndex++)
        {
            std::array<int, MAX_LIGHTMAP_PAGE_COUNT> vertexCountByLightmap{};

            for (auto surfaceIndex = 0uz; surfaceIndex < surfaceCount; surfaceIndex++)
            {
                const auto* record = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
                if (ReadU16(record) != materialIndex)
                    continue;

                const auto lightmapIndex = std::to_integer<unsigned>(record[2]);
                if (lightmapIndex == SKY_LIGHTMAP_INDEX)
                    continue;

                if (lightmapIndex >= rawPageCount)
                {
                    error = std::format("world surface {} references missing lightmap page {}", surfaceIndex, lightmapIndex);
                    return false;
                }

                vertexCountByLightmap[lightmapIndex] =
                    SaturatingAdd(vertexCountByLightmap[lightmapIndex], static_cast<int>(ReadU16(record, 16uz)));
            }

            for (auto left = 0u; left < rawPageCount; left++)
            {
                if (vertexCountByLightmap[left] == 0)
                    continue;

                for (auto right = left + 1u; right < rawPageCount; right++)
                {
                    if (vertexCountByLightmap[right] == 0)
                        continue;

                    const auto combinedWeight = SaturatingAdd(vertexCountByLightmap[left], vertexCountByLightmap[right]);
                    auto& leftToRight = coupling[left * MAX_LIGHTMAP_PAGE_COUNT + right];
                    leftToRight = SaturatingAdd(leftToRight, combinedWeight);
                    coupling[right * MAX_LIGHTMAP_PAGE_COUNT + left] = leftToRight;
                }
            }
        }

        return true;
    }

    [[nodiscard]] bool ReferencedLightmapPageCount(const IW3::d3dbsp::File& bsp, unsigned& pageCount, std::string& error)
    {
        pageCount = 0u;
        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        if (!surfaces)
            return true;

        if (surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz)
        {
            error = "world surface lump has funny size";
            return false;
        }

        const auto surfaceCount = RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE);
        for (auto surfaceIndex = 0uz; surfaceIndex < surfaceCount; surfaceIndex++)
        {
            const auto* record = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
            const auto lightmapIndex = std::to_integer<unsigned>(record[2]);
            if (lightmapIndex == SKY_LIGHTMAP_INDEX)
                continue;

            if (lightmapIndex >= MAX_LIGHTMAP_PAGE_COUNT)
            {
                error = std::format("world surface {} has invalid lightmap page {}", surfaceIndex, lightmapIndex);
                return false;
            }

            pageCount = std::max(pageCount, lightmapIndex + 1u);
        }

        return true;
    }

    [[nodiscard]] bool BuildLightmapAtlasLayout(const IW3::d3dbsp::File& bsp, LightmapAtlasLayout& layout, std::string& error)
    {
        const auto* lightmaps = bsp.GetLump(LUMP_LIGHTMAPS);
        if (!lightmaps || lightmaps->data.empty())
            return true;

        if (lightmaps->data.size() % LIGHTMAP_RAW_PAGE_SIZE != 0uz || !FitsUnsigned(lightmaps->data.size() / LIGHTMAP_RAW_PAGE_SIZE))
        {
            error = "lightmap lump has funny size";
            return false;
        }

        layout.rawPageCount = static_cast<unsigned>(lightmaps->data.size() / LIGHTMAP_RAW_PAGE_SIZE);
        if (layout.rawPageCount > MAX_LIGHTMAP_PAGE_COUNT)
        {
            error = std::format("lightmap lump has too many pages: {}", layout.rawPageCount);
            return false;
        }

        unsigned referencedPageCount = 0u;
        if (!ReferencedLightmapPageCount(bsp, referencedPageCount, error))
            return false;

        // linker_pc/Radiant treats the lightmap lump size and the highest
        // non-sky surface lightmap index as the same original-page count.
        if (referencedPageCount != layout.rawPageCount)
        {
            error = std::format("lightmap page count {} does not match surface references {}", layout.rawPageCount, referencedPageCount);
            return false;
        }

        std::array<int, MAX_LIGHTMAP_PAGE_COUNT * MAX_LIGHTMAP_PAGE_COUNT> coupling{};
        if (!BuildLightmapCouplingMatrix(bsp, layout.rawPageCount, coupling, error))
            return false;

        std::array<bool, MAX_LIGHTMAP_PAGE_COUNT> used{};
        layout.atlasIndexForRawPage.assign(layout.rawPageCount, 0u);
        layout.packedSlotForRawPage.assign(layout.rawPageCount, 0u);

        auto wideCount = 2u;
        auto highCount = 2u;
        auto packedRawPageCount = 0u;
        while (packedRawPageCount < layout.rawPageCount)
        {
            while (wideCount * highCount > layout.rawPageCount - packedRawPageCount)
            {
                if (wideCount < highCount)
                    highCount >>= 1u;
                else
                    wideCount >>= 1u;
            }

            LightmapAtlasGroup group;
            group.wideCount = std::max(1u, wideCount);
            group.highCount = std::max(1u, highCount);
            const auto groupPageCount = group.wideCount * group.highCount;
            group.rawPageForPackedSlot.reserve(groupPageCount);

            if (groupPageCount < 2u)
            {
                for (auto rawPage = 0u; rawPage < layout.rawPageCount; rawPage++)
                {
                    if (used[rawPage])
                        continue;

                    group.rawPageForPackedSlot.emplace_back(rawPage);
                    used[rawPage] = true;
                    break;
                }
            }
            else
            {
                auto bestLeft = SKY_LIGHTMAP_INDEX;
                auto bestRight = SKY_LIGHTMAP_INDEX;
                for (auto left = 0u; left + 1u < layout.rawPageCount; left++)
                {
                    if (used[left])
                        continue;

                    for (auto right = left + 1u; right < layout.rawPageCount; right++)
                    {
                        if (used[right])
                            continue;

                        if (bestLeft == SKY_LIGHTMAP_INDEX
                            || coupling[left * MAX_LIGHTMAP_PAGE_COUNT + right] > coupling[bestLeft * MAX_LIGHTMAP_PAGE_COUNT + bestRight])
                        {
                            bestLeft = left;
                            bestRight = right;
                        }
                    }
                }

                if (bestLeft == SKY_LIGHTMAP_INDEX || bestRight == SKY_LIGHTMAP_INDEX)
                {
                    error = "could not pair lightmap pages";
                    return false;
                }

                // The stock linker writes the selected pair into the atlas in
                // right,left order, then greedily extends that group from the
                // accumulated material-coupling weights.
                group.rawPageForPackedSlot.emplace_back(bestRight);
                group.rawPageForPackedSlot.emplace_back(bestLeft);
                used[bestRight] = true;
                used[bestLeft] = true;

                std::array<int, MAX_LIGHTMAP_PAGE_COUNT> aggregateWeights{};
                for (auto rawPage = 0u; rawPage < layout.rawPageCount; rawPage++)
                    aggregateWeights[rawPage] = coupling[bestLeft * MAX_LIGHTMAP_PAGE_COUNT + rawPage];

                auto selectedRawPage = bestRight;
                while (group.rawPageForPackedSlot.size() < groupPageCount)
                {
                    for (auto rawPage = 0u; rawPage < layout.rawPageCount; rawPage++)
                    {
                        aggregateWeights[rawPage] =
                            SaturatingAdd(aggregateWeights[rawPage], coupling[selectedRawPage * MAX_LIGHTMAP_PAGE_COUNT + rawPage]);
                    }

                    auto bestNext = SKY_LIGHTMAP_INDEX;
                    for (auto rawPage = 0u; rawPage < layout.rawPageCount; rawPage++)
                    {
                        if (used[rawPage])
                            continue;

                        if (bestNext == SKY_LIGHTMAP_INDEX || aggregateWeights[rawPage] > aggregateWeights[bestNext])
                            bestNext = rawPage;
                    }

                    if (bestNext == SKY_LIGHTMAP_INDEX)
                    {
                        error = "could not extend lightmap atlas group";
                        return false;
                    }

                    group.rawPageForPackedSlot.emplace_back(bestNext);
                    used[bestNext] = true;
                    selectedRawPage = bestNext;
                }
            }

            const auto atlasIndex = static_cast<unsigned>(layout.groups.size());
            for (auto packedSlot = 0uz; packedSlot < group.rawPageForPackedSlot.size(); packedSlot++)
            {
                const auto rawPage = group.rawPageForPackedSlot[packedSlot];
                layout.atlasIndexForRawPage[rawPage] = atlasIndex;
                layout.packedSlotForRawPage[rawPage] = static_cast<unsigned>(packedSlot);
            }

            packedRawPageCount += static_cast<unsigned>(group.rawPageForPackedSlot.size());
            layout.groups.emplace_back(std::move(group));
        }

        return true;
    }

    [[nodiscard]] std::vector<std::string> WorldMaterialNameCandidates(const std::string& rawMaterialName)
    {
        std::vector<std::string> result;

        if (rawMaterialName.empty())
            return result;

        // Raw v22 BSP world material names are stored as editor basenames
        // ("me_wire_black"), while the stock linker loads the Material asset
        // under the runtime name ("wc/me_wire_black"). Prefer the world
        // category first so optional probes do not emit a missing-material error
        // for the basename.
        if (!HasPathSeparator(rawMaterialName) && rawMaterialName[0] != '$')
            result.emplace_back(std::format("wc/{}", rawMaterialName));

        result.emplace_back(rawMaterialName);

        if (rawMaterialName == "$default")
        {
            result.emplace_back("$default3d");
            result.emplace_back("$default2d");
        }

        return result;
    }

    [[nodiscard]] XAssetInfo<Material>* TryLoadWorldMaterialDependency(AssetCreationContext& context, const std::string& materialName)
    {
        return static_cast<XAssetInfo<Material>*>(context.LoadDependencyGeneric(AssetMaterial::EnumEntry, materialName, false));
    }

    [[nodiscard]] XAssetInfo<Material>* GetOrCreateDefaultMaterialReference(AssetCreationContext& context, MemoryManager& memory)
    {
        if (auto* dependency = TryLoadWorldMaterialDependency(context, DEFAULT_MATERIAL_NAME))
            return dependency;

        // Raw BSPs can reference "$default" as a render material. The stock
        // game always provides that fallback material, so emit it as a fastfile
        // reference instead of requiring a local material file.
        auto* material = memory.Alloc<Material>();
        material->info.name = memory.Dup(DEFAULT_MATERIAL_REFERENCE_NAME);
        return context.AddAsset<AssetMaterial>(DEFAULT_MATERIAL_REFERENCE_NAME, material);
    }

    [[nodiscard]] std::vector<bool> WorldSurfaceMaterialUsage(const IW3::d3dbsp::File& bsp, const size_t materialCount, std::string& error)
    {
        std::vector<bool> result(materialCount);
        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        if (!surfaces)
            return result;

        if (surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz)
        {
            error = "world surface lump has funny size";
            return {};
        }

        const auto surfaceCount = RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE);
        for (auto surfaceIndex = 0uz; surfaceIndex < surfaceCount; surfaceIndex++)
        {
            const auto* record = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
            const auto materialIndex = static_cast<size_t>(ReadU16(record));
            if (materialIndex >= materialCount)
            {
                error = std::format("world surface {} references invalid material index {}", surfaceIndex, materialIndex);
                return {};
            }

            result[materialIndex] = true;
        }

        return result;
    }

    [[nodiscard]] std::vector<XAssetInfo<Material>*>
        LoadWorldMaterials(const IW3::d3dbsp::File& bsp, AssetCreationContext& context, MemoryManager& memory, std::string& error)
    {
        const auto* materials = bsp.GetLump(LUMP_MATERIALS);
        if (!ValidateRecordLump(bsp, materials, LUMP_MATERIALS, RAW_MATERIAL_SIZE, error))
            return {};

        std::vector<XAssetInfo<Material>*> result;
        const auto materialCount = RecordCount(*materials, RAW_MATERIAL_SIZE);
        const auto renderMaterialUsage = WorldSurfaceMaterialUsage(bsp, materialCount, error);
        if (!error.empty())
            return {};

        result.reserve(materialCount);

        for (auto materialIndex = 0uz; materialIndex < materialCount; materialIndex++)
        {
            const auto* record = materials->data.data() + materialIndex * RAW_MATERIAL_SIZE;
            auto materialName = RawMaterialName(record);
            if (materialName.empty())
            {
                if (renderMaterialUsage[materialIndex])
                {
                    error = std::format("world surface references unnamed material index {}", materialIndex);
                    return {};
                }

                result.emplace_back(nullptr);
                continue;
            }

            // The BSP material table is shared by render and collision data.
            // Tool-only entries such as "caulk" are valid in the table but are
            // not Material assets required by GfxWorld.
            if (!renderMaterialUsage[materialIndex])
            {
                result.emplace_back(nullptr);
                continue;
            }

            XAssetInfo<Material>* dependency = nullptr;
            if (materialName == DEFAULT_MATERIAL_NAME)
            {
                dependency = GetOrCreateDefaultMaterialReference(context, memory);
            }
            else
            {
                for (const auto& candidateName : WorldMaterialNameCandidates(materialName))
                {
                    dependency = TryLoadWorldMaterialDependency(context, candidateName);
                    if (dependency)
                        break;
                }
            }

            if (!dependency)
            {
                error = std::format("missing render material \"{}\"", materialName);
                return {};
            }

            result.emplace_back(dependency);
        }

        return result;
    }

    [[nodiscard]] std::vector<std::string> RawWorldMaterialNames(const IW3::d3dbsp::File& bsp, std::string& error)
    {
        const auto* materials = bsp.GetLump(LUMP_MATERIALS);
        if (!ValidateRecordLump(bsp, materials, LUMP_MATERIALS, RAW_MATERIAL_SIZE, error))
            return {};

        std::vector<std::string> result;
        const auto materialCount = RecordCount(*materials, RAW_MATERIAL_SIZE);
        result.reserve(materialCount);

        for (auto materialIndex = 0uz; materialIndex < materialCount; materialIndex++)
        {
            const auto* record = materials->data.data() + materialIndex * RAW_MATERIAL_SIZE;
            result.emplace_back(LowercaseAscii(RawMaterialName(record)));
        }

        return result;
    }

    void SetWorldBoundsFromVertices(GfxWorld& world)
    {
        if (!world.vd.vertices || world.vertexCount == 0u)
            return;

        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            world.mins[axis] = std::numeric_limits<float>::max();
            world.maxs[axis] = std::numeric_limits<float>::lowest();
        }

        for (auto vertexIndex = 0uz; vertexIndex < world.vertexCount; vertexIndex++)
        {
            const auto& vertex = world.vd.vertices[vertexIndex];
            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                world.mins[axis] = std::min(world.mins[axis], vertex.xyz[axis]);
                world.maxs[axis] = std::max(world.maxs[axis], vertex.xyz[axis]);
            }
        }
    }

    void ClearBounds(float (&mins)[3], float (&maxs)[3])
    {
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            mins[axis] = 131072.0f;
            maxs[axis] = -131072.0f;
        }
    }

    void ExpandBounds(const float (&addedMins)[3], const float (&addedMaxs)[3], float (&mins)[3], float (&maxs)[3])
    {
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            mins[axis] = std::min(mins[axis], addedMins[axis]);
            maxs[axis] = std::max(maxs[axis], addedMaxs[axis]);
        }
    }

    [[nodiscard]] bool PopulateWorldIndices(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* indices = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_INDICES, LUMP_SIMPLE_INDICES);
        if (!indices)
            return true;

        if (indices->data.size() % sizeof(uint16_t) != 0uz || !FitsInt(indices->data.size() / sizeof(uint16_t)))
        {
            error = "world index lump has funny size";
            return false;
        }

        world.indexCount = static_cast<int>(indices->data.size() / sizeof(uint16_t));
        world.indices = AllocCopy<uint16_t>(memory, indices->data);
        return true;
    }

    [[nodiscard]] bool PopulateWorldVertices(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* verts = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_VERTS, LUMP_SIMPLE_VERTS);
        if (!verts)
            return true;

        if (verts->data.size() % RAW_WORLD_VERTEX_SIZE != 0uz || !FitsUnsigned(RecordCount(*verts, RAW_WORLD_VERTEX_SIZE)))
        {
            error = "world vertex lump has funny size";
            return false;
        }

        world.vertexCount = static_cast<unsigned>(RecordCount(*verts, RAW_WORLD_VERTEX_SIZE));
        world.vd.vertices = AllocZeroed<GfxWorldVertex>(memory, world.vertexCount);

        for (auto vertexIndex = 0uz; vertexIndex < world.vertexCount; vertexIndex++)
        {
            const auto* record = verts->data.data() + vertexIndex * RAW_WORLD_VERTEX_SIZE;
            auto& vertex = world.vd.vertices[vertexIndex];
            float normal[3]{};
            float tangent[3]{};
            float binormal[3]{};
            float expectedBinormal[3]{};

            CopyFloat3(record, vertex.xyz);
            CopyFloat3(record + 12uz, normal);
            vertex.color.packed = ReadU32(record, 24uz);
            std::memcpy(vertex.texCoord, record + 28uz, sizeof(vertex.texCoord));
            std::memcpy(vertex.lmapCoord, record + 36uz, sizeof(vertex.lmapCoord));
            CopyFloat3(record + 44uz, tangent);
            CopyFloat3(record + 56uz, binormal);

            CrossProduct(normal, tangent, expectedBinormal);
            vertex.binormalSign = DotProduct(expectedBinormal, binormal) < 0.0f ? -1.0f : 1.0f;
            vertex.normal = PackRawBspUnitVec(normal);
            vertex.tangent = PackRawBspUnitVec(tangent);
        }

        SetWorldBoundsFromVertices(world);
        return true;
    }

    void PopulateSurfaceBounds(GfxWorld& world, GfxSurface& surface)
    {
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            surface.bounds[0][axis] = std::numeric_limits<float>::max();
            surface.bounds[1][axis] = std::numeric_limits<float>::lowest();
        }

        auto foundVertex = false;
        const auto firstIndex = surface.tris.baseIndex;
        const auto indexCount = static_cast<int>(surface.tris.triCount) * 3;
        if (world.indices && world.vd.vertices && firstIndex >= 0 && indexCount > 0 && firstIndex + indexCount <= world.indexCount)
        {
            for (auto indexOffset = 0; indexOffset < indexCount; indexOffset++)
            {
                const auto vertexIndex = surface.tris.firstVertex + world.indices[firstIndex + indexOffset];
                if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                    continue;

                const auto& vertex = world.vd.vertices[vertexIndex];
                for (auto axis = 0uz; axis < 3uz; axis++)
                {
                    surface.bounds[0][axis] = std::min(surface.bounds[0][axis], vertex.xyz[axis]);
                    surface.bounds[1][axis] = std::max(surface.bounds[1][axis], vertex.xyz[axis]);
                }
                foundVertex = true;
            }
        }

        if (!foundVertex)
        {
            for (auto axis = 0uz; axis < 3uz; axis++)
            {
                surface.bounds[0][axis] = world.mins[axis];
                surface.bounds[1][axis] = world.maxs[axis];
            }
        }
    }

    void ApplySurfaceLightmapRemap(
        GfxWorld& world,
        const GfxSurface& surface,
        const LightmapAtlasGroup& group,
        const unsigned packedSlot,
        std::vector<int>& vertexLightmapRemaps)
    {
        if (group.wideCount * group.highCount <= 1u || !world.indices || !world.vd.vertices)
            return;

        const auto firstIndex = surface.tris.baseIndex;
        const auto indexCount = static_cast<int>(surface.tris.triCount) * 3;
        if (firstIndex < 0 || indexCount <= 0 || firstIndex + indexCount > world.indexCount)
            return;

        const auto slotX = packedSlot % group.wideCount;
        const auto slotY = packedSlot / group.wideCount;
        const auto scaleU = LinkerFloat(1.0 / static_cast<double>(group.wideCount));
        const auto scaleV = LinkerFloat(1.0 / static_cast<double>(group.highCount));
        const auto offsetU = LinkerFloat(static_cast<double>(slotX) * scaleU);
        const auto offsetV = LinkerFloat(static_cast<double>(slotY) * scaleV);

        for (auto indexOffset = 0; indexOffset < indexCount; indexOffset++)
        {
            const auto vertexIndex = surface.tris.firstVertex + world.indices[firstIndex + indexOffset];
            if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                continue;

            // Raw BSP vertices are shared by all surfaces that reference them.
            // Applying the same atlas scale/offset repeatedly corrupts the UVs
            // and later dumps as black lightmap stripes in Radiant.
            auto& vertexRemap = vertexLightmapRemaps[vertexIndex];
            if (vertexRemap >= 0)
                continue;

            vertexRemap = static_cast<int>(packedSlot);
            auto& vertex = world.vd.vertices[vertexIndex];
            vertex.lmapCoord[0] = LinkerFloat(static_cast<double>(vertex.lmapCoord[0]) * scaleU + offsetU);
            vertex.lmapCoord[1] = LinkerFloat(static_cast<double>(vertex.lmapCoord[1]) * scaleV + offsetV);
        }
    }

    [[nodiscard]] bool ApplyMagicPortalVertexCoords(GfxWorld& world, const GfxSurface& surface, std::string& error)
    {
        if (!surface.material || (surface.material->info.gameFlags & MATERIAL_GAME_FLAG_MAGIC_PORTAL) == 0u)
            return true;

        const auto triCount = static_cast<size_t>(surface.tris.triCount);
        const auto indexCount = triCount * 3uz;
        if (triCount == 0uz)
            return true;

        if (!world.indices || !world.vd.vertices || surface.tris.baseIndex < 0 || static_cast<size_t>(surface.tris.baseIndex) > static_cast<size_t>(world.indexCount)
            || indexCount > static_cast<size_t>(world.indexCount) - static_cast<size_t>(surface.tris.baseIndex))
        {
            error = "magic portal surface index range is out of bounds";
            return false;
        }

        std::vector<size_t> fillId(triCount);
        std::vector<std::array<float, 3>> centerAccum(triCount);
        std::vector<float> centerWeight(triCount);
        for (auto triIndex = 0uz; triIndex < triCount; triIndex++)
            fillId[triIndex] = triIndex;

        // Stock R_SurfCalculateMagicPortalVerts groups connected triangles by
        // shared vertex, averages each connected component's xyz center, then
        // stores that center as texCoord.xy/lmapCoord.x with lmapCoord.y = 1.
        // These portal materials do not preserve authored UVs in the runtime
        // world and must be normalized this way to dump back to linker_pc form.
        auto changed = true;
        while (changed)
        {
            changed = false;
            for (auto leftTri = 0uz; leftTri < triCount; leftTri++)
            {
                std::array<int, 3> leftVerts{};
                for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
                    leftVerts[triVertex] = surface.tris.firstVertex + world.indices[surface.tris.baseIndex + static_cast<int>(leftTri * 3uz + triVertex)];

                for (auto rightTri = 0uz; rightTri < triCount; rightTri++)
                {
                    std::array<int, 3> rightVerts{};
                    for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
                        rightVerts[triVertex] = surface.tris.firstVertex + world.indices[surface.tris.baseIndex + static_cast<int>(rightTri * 3uz + triVertex)];

                    for (const auto leftVertex : leftVerts)
                    {
                        for (const auto rightVertex : rightVerts)
                        {
                            if (leftVertex != rightVertex || fillId[leftTri] == fillId[rightTri])
                                continue;

                            if (fillId[leftTri] >= fillId[rightTri])
                                fillId[leftTri] = fillId[rightTri];
                            else
                                fillId[rightTri] = fillId[leftTri];
                            changed = true;
                        }
                    }
                }
            }
        }

        for (auto triIndex = 0uz; triIndex < triCount; triIndex++)
        {
            const auto targetFill = fillId[triIndex];
            for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
            {
                const auto vertexIndex = surface.tris.firstVertex + world.indices[surface.tris.baseIndex + static_cast<int>(triIndex * 3uz + triVertex)];
                if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                {
                    error = "magic portal surface references invalid vertex";
                    return false;
                }

                const auto& vertex = world.vd.vertices[vertexIndex];
                for (auto axis = 0uz; axis < 3uz; axis++)
                    centerAccum[targetFill][axis] = LinkerFloat(static_cast<double>(centerAccum[targetFill][axis]) + vertex.xyz[axis]);
                centerWeight[targetFill] = LinkerFloat(static_cast<double>(centerWeight[targetFill]) + 1.0);
            }
        }

        for (auto triIndex = 0uz; triIndex < triCount; triIndex++)
        {
            if (centerWeight[triIndex] <= 0.0f)
                continue;

            const auto scale = LinkerFloat(1.0 / static_cast<double>(centerWeight[triIndex]));
            for (auto axis = 0uz; axis < 3uz; axis++)
                centerAccum[triIndex][axis] = LinkerFloat(static_cast<double>(centerAccum[triIndex][axis]) * scale);
        }

        for (auto triIndex = 0uz; triIndex < triCount; triIndex++)
        {
            const auto sourceFill = fillId[triIndex];
            for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
            {
                const auto vertexIndex = surface.tris.firstVertex + world.indices[surface.tris.baseIndex + static_cast<int>(triIndex * 3uz + triVertex)];
                auto& vertex = world.vd.vertices[vertexIndex];
                vertex.texCoord[0] = centerAccum[sourceFill][0];
                vertex.texCoord[1] = centerAccum[sourceFill][1];
                vertex.lmapCoord[0] = centerAccum[sourceFill][2];
                vertex.lmapCoord[1] = 1.0f;
            }
        }

        return true;
    }

    struct RawWorldSurfaceIndexInfo
    {
        uint16_t materialIndex = 0u;
        uint8_t lightmapIndex = 0u;
        uint8_t reflectionProbeIndex = 0u;
        int firstIndex = 0;
        uint16_t indexCount = 0u;
    };

    [[nodiscard]] bool RawSurfaceMaterialsMatch(
        const RawWorldSurfaceIndexInfo& left,
        const RawWorldSurfaceIndexInfo& right,
        const size_t firstSurfaceIndex,
        const std::vector<std::string>& rawMaterialNames)
    {
        if (left.materialIndex == right.materialIndex)
            return true;

        if (left.materialIndex >= rawMaterialNames.size() || firstSurfaceIndex >= rawMaterialNames.size())
            return false;

        // linker_pc's R_LoadSurfaces fallback compares the candidate material
        // name to materialTable[firstSurfaceIndex], not to
        // materialTable[firstSurface.materialIndex]. This odd-looking offset is
        // visible in the decomp and is required to reproduce the canonical
        // runtime index-buffer order.
        return rawMaterialNames[left.materialIndex] == rawMaterialNames[firstSurfaceIndex];
    }

    [[nodiscard]] bool RawSurfaceIndexGroupsMatch(
        const RawWorldSurfaceIndexInfo& left,
        const RawWorldSurfaceIndexInfo& right,
        const size_t firstSurfaceIndex,
        const std::vector<std::string>& rawMaterialNames)
    {
        return RawSurfaceMaterialsMatch(left, right, firstSurfaceIndex, rawMaterialNames)
               && left.reflectionProbeIndex == right.reflectionProbeIndex
               && left.lightmapIndex == right.lightmapIndex;
    }

    [[nodiscard]] bool RewriteWorldIndicesLikeLinker(
        GfxWorld& world,
        const std::vector<RawWorldSurfaceIndexInfo>& rawSurfaces,
        const std::vector<std::string>& rawMaterialNames,
        MemoryManager& memory,
        std::string& error)
    {
        if (rawSurfaces.empty())
            return true;

        if (!world.indices)
        {
            error = "world surfaces require an index lump";
            return false;
        }

        auto rewrittenIndexCount = 0uz;
        for (const auto& surface : rawSurfaces)
            rewrittenIndexCount += surface.indexCount;

        if (!FitsInt(rewrittenIndexCount))
        {
            error = "world index count is too large";
            return false;
        }

        std::vector<uint16_t> rewrittenIndices(rewrittenIndexCount);
        std::vector<bool> assigned(rawSurfaces.size());
        auto writeIndex = 0uz;

        // linker_pc does not keep the raw draw-index lump as-is. During
        // R_LoadSurfaces it walks raw surfaces in order, groups still-unwritten
        // surfaces by raw material name, raw reflection probe, and raw lightmap
        // page, and appends each raw index span into a canonical runtime index
        // buffer. The surface baseIndex values then point into this rewritten
        // buffer before R_SortSurfaces moves the surface records.
        for (auto firstSurface = 0uz; firstSurface < rawSurfaces.size(); firstSurface++)
        {
            if (assigned[firstSurface])
                continue;

            const auto& first = rawSurfaces[firstSurface];
            for (auto surfaceIndex = firstSurface; surfaceIndex < rawSurfaces.size(); surfaceIndex++)
            {
                if (assigned[surfaceIndex] || !RawSurfaceIndexGroupsMatch(rawSurfaces[surfaceIndex], first, firstSurface, rawMaterialNames))
                    continue;

                const auto& rawSurface = rawSurfaces[surfaceIndex];
                if (rawSurface.firstIndex < 0 || static_cast<size_t>(rawSurface.firstIndex) > static_cast<size_t>(world.indexCount)
                    || static_cast<size_t>(rawSurface.indexCount) > static_cast<size_t>(world.indexCount) - static_cast<size_t>(rawSurface.firstIndex))
                {
                    error = std::format("world surface {} index range is out of bounds", surfaceIndex);
                    return false;
                }

                if (writeIndex + rawSurface.indexCount > rewrittenIndices.size())
                {
                    error = "world index rewrite exceeded output size";
                    return false;
                }

                std::memcpy(&rewrittenIndices[writeIndex], &world.indices[rawSurface.firstIndex], static_cast<size_t>(rawSurface.indexCount) * sizeof(uint16_t));
                world.dpvs.surfaces[surfaceIndex].tris.baseIndex = static_cast<int>(writeIndex);
                assigned[surfaceIndex] = true;
                writeIndex += rawSurface.indexCount;
            }
        }

        if (writeIndex != rewrittenIndices.size())
        {
            error = "world index rewrite did not write every index";
            return false;
        }

        world.indexCount = static_cast<int>(rewrittenIndices.size());
        world.indices = memory.Alloc<uint16_t>(rewrittenIndices.size());
        if (world.indices && !rewrittenIndices.empty())
            std::memcpy(world.indices, rewrittenIndices.data(), rewrittenIndices.size() * sizeof(uint16_t));

        return true;
    }

    [[nodiscard]] bool PopulateWorldSurfaces(
        GfxWorld& world,
        const IW3::d3dbsp::File& bsp,
        const LightmapAtlasLayout& lightmapLayout,
        const std::vector<XAssetInfo<Material>*>& materialDependencies,
        MemoryManager& memory,
        std::string& error)
    {
        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        if (!surfaces)
            return true;

        if (surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz || !FitsInt(RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE)))
        {
            error = "world surface lump has funny size";
            return false;
        }

        world.surfaceCount = static_cast<int>(RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE));
        world.dpvs.staticSurfaceCount = static_cast<unsigned>(world.surfaceCount);
        world.dpvs.staticSurfaceCountNoDecal = static_cast<unsigned>(world.surfaceCount);
        world.dpvs.litSurfsBegin = 0u;
        world.dpvs.litSurfsEnd = static_cast<unsigned>(world.surfaceCount);
        world.dpvs.surfaces = AllocZeroed<GfxSurface>(memory, world.surfaceCount);
        std::vector<int> vertexLightmapRemaps(world.vertexCount, -1);
        std::vector<RawWorldSurfaceIndexInfo> rawSurfaceIndexInfo(static_cast<size_t>(world.surfaceCount));
        auto rawMaterialNames = RawWorldMaterialNames(bsp, error);
        if (!error.empty())
            return false;

        for (auto surfaceIndex = 0uz; surfaceIndex < static_cast<size_t>(world.surfaceCount); surfaceIndex++)
        {
            const auto* record = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
            auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto materialIndex = ReadU16(record);
            if (materialIndex >= materialDependencies.size() || !materialDependencies[materialIndex])
            {
                error = std::format("world surface {} references missing render material index {}", surfaceIndex, materialIndex);
                return false;
            }

            surface.material = materialDependencies[materialIndex]->Asset();

            const auto rawLightmapIndex = std::to_integer<unsigned>(record[2]);
            surface.lightmapIndex = static_cast<char>(rawLightmapIndex);
            surface.reflectionProbeIndex = static_cast<char>(std::to_integer<unsigned char>(record[3]));
            surface.primaryLightIndex = static_cast<char>(std::to_integer<unsigned char>(record[4]));
            surface.flags = static_cast<char>(std::to_integer<unsigned char>(record[5]));
            surface.tris.vertexLayerData = ReadI32(record, 8uz);
            surface.tris.firstVertex = ReadI32(record, 12uz);
            surface.tris.vertexCount = ReadU16(record, 16uz);
            surface.tris.triCount = static_cast<uint16_t>(ReadU16(record, 18uz) / 3u);
            surface.tris.baseIndex = -1;

            rawSurfaceIndexInfo[surfaceIndex].materialIndex = materialIndex;
            rawSurfaceIndexInfo[surfaceIndex].lightmapIndex = static_cast<uint8_t>(rawLightmapIndex);
            rawSurfaceIndexInfo[surfaceIndex].reflectionProbeIndex = U8(surface.reflectionProbeIndex);
            rawSurfaceIndexInfo[surfaceIndex].firstIndex = ReadI32(record, 20uz);
            rawSurfaceIndexInfo[surfaceIndex].indexCount = ReadU16(record, 18uz);
        }

        if (!RewriteWorldIndicesLikeLinker(world, rawSurfaceIndexInfo, rawMaterialNames, memory, error))
            return false;

        for (auto surfaceIndex = 0uz; surfaceIndex < static_cast<size_t>(world.surfaceCount); surfaceIndex++)
        {
            auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto rawLightmapIndex = rawSurfaceIndexInfo[surfaceIndex].lightmapIndex;
            if (rawLightmapIndex != SKY_LIGHTMAP_INDEX && rawLightmapIndex < lightmapLayout.atlasIndexForRawPage.size())
            {
                const auto atlasIndex = lightmapLayout.atlasIndexForRawPage[rawLightmapIndex];
                const auto packedSlot = lightmapLayout.packedSlotForRawPage[rawLightmapIndex];
                if (atlasIndex < lightmapLayout.groups.size())
                {
                    // Raw BSP surfaces reference original lightmap pages. The
                    // linker replaces that with the merged runtime atlas index
                    // and folds the original page slot into vertex lmap UVs.
                    surface.lightmapIndex = static_cast<char>(atlasIndex);
                    ApplySurfaceLightmapRemap(world, surface, lightmapLayout.groups[atlasIndex], packedSlot, vertexLightmapRemaps);
                }
            }

            if (!ApplyMagicPortalVertexCoords(world, surface, error))
                return false;

            PopulateSurfaceBounds(world, surface);
        }

        return true;
    }

    void PopulateWorldMaterialMemory(GfxWorld& world, MemoryManager& memory)
    {
        struct MaterialUsage
        {
            Material* material = nullptr;
            int memory = 0;
            std::vector<int> firstVertices;
        };

        std::array<MaterialUsage, MATERIAL_USAGE_HASH_SIZE> usages;
        std::array<Material*, MATERIAL_USAGE_HASH_SIZE> materialHashTable{};
        if (!world.dpvs.surfaces || world.surfaceCount <= 0)
            return;

        for (auto surfaceIndex = 0; surfaceIndex < world.surfaceCount; surfaceIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            auto* material = surface.material;
            const auto materialHashIndex = MaterialHashIndex(materialHashTable, material);
            if (!materialHashIndex)
                continue;

            auto& usage = usages[*materialHashIndex];
            usage.material = material;
            // R_MaterialUsage accounts for the surface header, index payload,
            // and a fixed per-surface cost. Vertex data is charged once for
            // each unique firstVertex used by that material.
            usage.memory += static_cast<int>(6u * surface.tris.triCount + 16u + 48u);

            const auto existingVertexRange = std::find(usage.firstVertices.begin(), usage.firstVertices.end(), surface.tris.firstVertex);
            if (existingVertexRange == usage.firstVertices.end())
            {
                usage.firstVertices.emplace_back(surface.tris.firstVertex);
                usage.memory += 44 * surface.tris.vertexCount;
            }
        }

        auto materialMemoryCount = 0uz;
        for (const auto& usage : usages)
        {
            if (usage.memory != 0)
                materialMemoryCount++;
        }

        if (materialMemoryCount == 0uz)
            return;

        world.materialMemoryCount = static_cast<int>(materialMemoryCount);
        world.materialMemory = AllocZeroed<MaterialMemory>(memory, materialMemoryCount);

        auto outIndex = 0uz;
        for (const auto& usage : usages)
        {
            if (usage.memory == 0)
                continue;

            world.materialMemory[outIndex].material = usage.material;
            world.materialMemory[outIndex].memory = usage.memory;
            outIndex++;
        }
    }

    [[nodiscard]] bool PopulateWorldVertexLayerData(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* vertexLayerData = bsp.GetLump(LUMP_VERTEX_LAYER_DATA);
        if (UsesSimpleWorldGeometry(bsp) || !vertexLayerData || vertexLayerData->data.empty())
        {
            // linker_pc only copies LUMP_VERTEX_LAYER_DATA for the layered
            // geometry path. Simple/unlayered BSP geometry uses a 4-byte dummy
            // layer buffer even when the raw file still contains lump 42.
            world.vertexLayerDataSize = 4u;
            world.vld.data = AllocZeroed<char>(memory, world.vertexLayerDataSize);
            return true;
        }

        if (!FitsUnsigned(vertexLayerData->data.size()))
        {
            error = "vertex layer data lump is too large";
            return false;
        }

        world.vertexLayerDataSize = static_cast<unsigned>(vertexLayerData->data.size());
        world.vld.data = AllocCopy<char>(memory, vertexLayerData->data);
        return true;
    }

    [[nodiscard]] XAssetInfo<GfxImage>*
        AddGeneratedImage(AssetCreationContext& context, AssetRegistration<AssetGfxWorld>& registration, const std::string& imageName, GfxImage* image)
    {
        auto* imageInfo = context.AddAsset<AssetImage>(imageName, image);
        if (imageInfo)
            registration.AddDependency(imageInfo);
        return imageInfo;
    }

    [[nodiscard]] std::string LightmapImageName(const unsigned lightmapIndex, const std::string_view suffix)
    {
        return std::format("*lightmap{}_{}", lightmapIndex, suffix);
    }

    void CopyPrimaryLightmapRawPageToAtlas(
        std::vector<std::byte>& out, const std::byte* page, const LightmapAtlasGroup& group, const unsigned packedSlot)
    {
        const auto atlasWidth = static_cast<size_t>(group.wideCount) * LIGHTMAP_PRIMARY_RAW_WIDTH;
        const auto slotX = packedSlot % group.wideCount;
        const auto slotY = packedSlot / group.wideCount;
        const auto* source = page + LIGHTMAP_SECONDARY_RAW_PAGE_SIZE;
        const auto destinationX = static_cast<size_t>(slotX) * LIGHTMAP_PRIMARY_RAW_WIDTH;
        const auto destinationY = static_cast<size_t>(slotY) * LIGHTMAP_PRIMARY_RAW_HEIGHT;

        for (auto row = 0u; row < LIGHTMAP_PRIMARY_RAW_HEIGHT; row++)
        {
            const auto destinationOffset = (destinationY + row) * atlasWidth + destinationX;
            std::memcpy(out.data() + destinationOffset, source + static_cast<size_t>(row) * LIGHTMAP_PRIMARY_RAW_WIDTH, LIGHTMAP_PRIMARY_RAW_WIDTH);
        }
    }

    void CopySecondaryLightmapRawPageToAtlas(
        std::vector<std::byte>& out, const std::byte* page, const LightmapAtlasGroup& group, const unsigned packedSlot)
    {
        const auto atlasStride = static_cast<size_t>(group.wideCount) * LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE;
        const auto slotX = packedSlot % group.wideCount;
        const auto slotY = packedSlot / group.wideCount;
        const auto destinationX = static_cast<size_t>(slotX) * LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE;
        constexpr auto rowSize = static_cast<size_t>(LIGHTMAP_SECONDARY_RAW_WIDTH) * LIGHTMAP_SECONDARY_PIXEL_SIZE;

        // Runtime secondary lightmaps keep all page top halves first, followed
        // by all bottom halves. This is the inverse of the raw page layout.
        for (auto row = 0u; row < LIGHTMAP_SECONDARY_HALF_HEIGHT; row++)
        {
            const auto destinationY = static_cast<size_t>(slotY) * LIGHTMAP_SECONDARY_HALF_HEIGHT + row;
            const auto destinationOffset = destinationY * atlasStride + destinationX;
            std::memcpy(out.data() + destinationOffset, page + static_cast<size_t>(row) * rowSize, rowSize);
        }

        for (auto row = 0u; row < LIGHTMAP_SECONDARY_HALF_HEIGHT; row++)
        {
            const auto destinationY = static_cast<size_t>(group.highCount + slotY) * LIGHTMAP_SECONDARY_HALF_HEIGHT + row;
            const auto destinationOffset = destinationY * atlasStride + destinationX;
            const auto sourceOffset = static_cast<size_t>(LIGHTMAP_SECONDARY_HALF_HEIGHT + row) * rowSize;
            std::memcpy(out.data() + destinationOffset, page + sourceOffset, rowSize);
        }
    }

    [[nodiscard]] bool IsUniformReflectionProbePixels(const std::byte* source, const uint8_t b, const uint8_t g, const uint8_t r, const uint8_t a)
    {
        const auto pixelCount = REFLECTION_PROBE_RAW_DATA_SIZE / sizeof(uint32_t);
        for (auto pixelIndex = 0uz; pixelIndex < pixelCount; pixelIndex++)
        {
            const auto* pixel = source + pixelIndex * sizeof(uint32_t);
            if (std::to_integer<uint8_t>(pixel[0]) != b || std::to_integer<uint8_t>(pixel[1]) != g || std::to_integer<uint8_t>(pixel[2]) != r
                || std::to_integer<uint8_t>(pixel[3]) != a)
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool IsPlaceholderReflectionProbePixels(const std::byte* source)
    {
        return IsUniformReflectionProbePixels(source, 0u, 0u, 0u, 0u) || IsUniformReflectionProbePixels(source, 0x48u, 0x48u, 0x48u, 0u);
    }

    void FillDefaultAuthoredReflectionProbePixels(char* out)
    {
        const auto pixelCount = REFLECTION_PROBE_RAW_DATA_SIZE / sizeof(uint32_t);
        for (auto pixelIndex = 0uz; pixelIndex < pixelCount; pixelIndex++)
        {
            auto* pixel = out + pixelIndex * sizeof(uint32_t);
            pixel[0] = 0;
            pixel[1] = 0;
            pixel[2] = static_cast<char>(0xFF);
            pixel[3] = static_cast<char>(0xFF);
        }
    }

    [[nodiscard]] std::optional<std::pair<std::vector<std::byte>, std::vector<std::byte>>> BuildLightmapAtlasImages(
        const IW3::d3dbsp::Lump& lightmaps,
        const LightmapAtlasGroup& group,
        const std::vector<GfxLightDef*>& lightDefs,
        ISearchPath& searchPath,
        std::string& error)
    {
        const auto primaryAtlasSize =
            static_cast<size_t>(group.wideCount) * LIGHTMAP_PRIMARY_RAW_WIDTH * static_cast<size_t>(group.highCount) * LIGHTMAP_PRIMARY_RAW_HEIGHT;
        const auto secondaryAtlasSize = static_cast<size_t>(group.wideCount) * LIGHTMAP_SECONDARY_RAW_WIDTH * LIGHTMAP_SECONDARY_PIXEL_SIZE
                                        * static_cast<size_t>(group.highCount) * LIGHTMAP_SECONDARY_RAW_HEIGHT;

        std::vector<std::byte> primary(primaryAtlasSize);
        std::vector<std::byte> secondary(secondaryAtlasSize);

        for (auto packedSlot = 0uz; packedSlot < group.rawPageForPackedSlot.size(); packedSlot++)
        {
            const auto rawPage = group.rawPageForPackedSlot[packedSlot];
            const auto* page = lightmaps.data.data() + static_cast<size_t>(rawPage) * LIGHTMAP_RAW_PAGE_SIZE;
            CopySecondaryLightmapRawPageToAtlas(secondary, page, group, static_cast<unsigned>(packedSlot));
            CopyPrimaryLightmapRawPageToAtlas(primary, page, group, static_cast<unsigned>(packedSlot));
        }

        if (!ApplyLightDefAttenuationImages(secondary, group, lightDefs, searchPath, error))
            return std::nullopt;

        return std::make_pair(std::move(primary), std::move(secondary));
    }

    [[nodiscard]] bool PopulateWorldLightmaps(
        GfxWorld& world,
        const IW3::d3dbsp::File& bsp,
        const LightmapAtlasLayout& lightmapLayout,
        const std::vector<GfxLightDef*>& lightDefs,
        ISearchPath& searchPath,
        AssetCreationContext& context,
        AssetRegistration<AssetGfxWorld>& registration,
        MemoryManager& memory,
        std::string& error)
    {
        const auto* lightmaps = bsp.GetLump(LUMP_LIGHTMAPS);
        if (!lightmaps || lightmaps->data.empty())
            return true;

        if (lightmaps->data.size() % LIGHTMAP_RAW_PAGE_SIZE != 0uz || !FitsInt(lightmaps->data.size() / LIGHTMAP_RAW_PAGE_SIZE))
        {
            error = "lightmap lump has funny size";
            return false;
        }

        const auto pageCount = static_cast<unsigned>(lightmaps->data.size() / LIGHTMAP_RAW_PAGE_SIZE);
        if (pageCount != lightmapLayout.rawPageCount)
        {
            error = "lightmap atlas layout does not match lightmap lump";
            return false;
        }

        world.lightmapCount = static_cast<int>(lightmapLayout.groups.size());
        world.lightmaps = AllocZeroed<GfxLightmapArray>(memory, lightmapLayout.groups.size());
        // These arrays are runtime handles. The fastfile DB loader only
        // allocates them when the serialized pointers are non-null; R_LoadWorld
        // fills them from the generated lightmap images after loading.
        world.lightmapPrimaryTextures = AllocZeroed<GfxTexture>(memory, lightmapLayout.groups.size());
        world.lightmapSecondaryTextures = AllocZeroed<GfxTexture>(memory, lightmapLayout.groups.size());

        for (auto lightmapIndex = 0uz; lightmapIndex < lightmapLayout.groups.size(); lightmapIndex++)
        {
            const auto& group = lightmapLayout.groups[lightmapIndex];
            auto atlasImages = BuildLightmapAtlasImages(*lightmaps, group, lightDefs, searchPath, error);
            if (!atlasImages)
                return false;

            const auto& primaryPixels = atlasImages->first;
            const auto& secondaryPixels = atlasImages->second;
            const auto primaryName = LightmapImageName(static_cast<unsigned>(lightmapIndex), "primary");
            const auto secondaryName = LightmapImageName(static_cast<unsigned>(lightmapIndex), "secondary");
            constexpr auto lightmapFlags = static_cast<char>(image::iwi6::IMG_FLAG_NOMIPMAPS);

            auto* primary = CreateGeneratedImage(memory,
                                                 primaryName,
                                                 MAPTYPE_2D,
                                                 TS_FUNCTION,
                                                 IMG_CATEGORY_LIGHTMAP,
                                                 static_cast<uint16_t>(group.wideCount * LIGHTMAP_PRIMARY_RAW_WIDTH),
                                                 static_cast<uint16_t>(group.highCount * LIGHTMAP_PRIMARY_RAW_HEIGHT),
                                                 1u,
                                                 oat::D3DFMT_L8,
                                                 lightmapFlags,
                                                 primaryPixels.data(),
                                                 primaryPixels.size());
            auto* secondary = CreateGeneratedImage(memory,
                                                   secondaryName,
                                                   MAPTYPE_2D,
                                                   TS_FUNCTION,
                                                   IMG_CATEGORY_LIGHTMAP,
                                                   static_cast<uint16_t>(group.wideCount * LIGHTMAP_SECONDARY_RAW_WIDTH),
                                                   static_cast<uint16_t>(group.highCount * LIGHTMAP_SECONDARY_RAW_HEIGHT),
                                                   1u,
                                                   oat::D3DFMT_A8R8G8B8,
                                                   lightmapFlags,
                                                   secondaryPixels.data(),
                                                   secondaryPixels.size());

            auto* primaryInfo = AddGeneratedImage(context, registration, primaryName, primary);
            auto* secondaryInfo = AddGeneratedImage(context, registration, secondaryName, secondary);
            if (!primaryInfo || !secondaryInfo)
            {
                error = "could not register generated lightmap image";
                return false;
            }

            world.lightmaps[lightmapIndex].primary = primaryInfo->Asset();
            world.lightmaps[lightmapIndex].secondary = secondaryInfo->Asset();
        }

        return true;
    }

    void CopyTransformedReflectionProbePixels(char* out, const std::byte* source)
    {
        // cod4map can emit placeholder probe pixels when reflections were not
        // generated. linker_pc normalizes those authored probes to an opaque
        // blue fallback image; preserving/color-correcting the placeholder would
        // produce black probes and a non-canonical d3dbsp after dumping.
        if (IsPlaceholderReflectionProbePixels(source))
        {
            FillDefaultAuthoredReflectionProbePixels(out);
            return;
        }

        const auto pixelCount = REFLECTION_PROBE_RAW_DATA_SIZE / sizeof(uint32_t);
        for (auto pixelIndex = 0uz; pixelIndex < pixelCount; pixelIndex++)
        {
            const auto* pixel = source + pixelIndex * sizeof(uint32_t);
            const auto transformed = TransformReflectionProbeColor(std::to_integer<uint8_t>(pixel[0]),
                                                                   std::to_integer<uint8_t>(pixel[1]),
                                                                   std::to_integer<uint8_t>(pixel[2]));
            std::memcpy(out + pixelIndex * sizeof(uint32_t), &transformed, sizeof(transformed));
        }
    }

    [[nodiscard]] bool CreateDefaultReflectionProbe(
        GfxWorld& world, AssetCreationContext& context, AssetRegistration<AssetGfxWorld>& registration, MemoryManager& memory, std::string& error)
    {
        auto* image = CreateGeneratedImage(memory,
                                           "*reflection_probe0",
                                           MAPTYPE_CUBE,
                                           TS_COLOR_MAP,
                                           IMG_CATEGORY_AUTO_GENERATED,
                                           REFLECTION_PROBE_SIZE,
                                           REFLECTION_PROBE_SIZE,
                                           1u,
                                           oat::D3DFMT_A8R8G8B8,
                                           static_cast<char>(image::iwi6::IMG_FLAG_CUBEMAP),
                                           nullptr,
                                           REFLECTION_PROBE_RAW_DATA_SIZE);

        // R_CreateDefaultProbe fills the raw probe with 0xFFFF0000 pixels, then
        // runs it through the same reflection image generation path as authored
        // probes. In raw byte order that is RGB 0,0,255.
        const auto transformed = TransformReflectionProbeColor(0u, 0u, 255u);
        for (auto pixelOffset = 0uz; pixelOffset < REFLECTION_PROBE_RAW_DATA_SIZE; pixelOffset += sizeof(uint32_t))
            std::memcpy(image->texture.loadDef->data + pixelOffset, &transformed, sizeof(transformed));

        auto* imageInfo = AddGeneratedImage(context, registration, image->name, image);
        if (!imageInfo)
        {
            error = "could not register generated default reflection probe image";
            return false;
        }

        world.reflectionProbes[0].reflectionImage = imageInfo->Asset();
        return true;
    }

    [[nodiscard]] bool PopulateWorldReflectionProbes(
        GfxWorld& world,
        const IW3::d3dbsp::File& bsp,
        AssetCreationContext& context,
        AssetRegistration<AssetGfxWorld>& registration,
        MemoryManager& memory,
        std::string& error)
    {
        const auto* reflectionProbes = bsp.GetLump(LUMP_REFLECTION_PROBES);
        if (!reflectionProbes || reflectionProbes->data.empty())
        {
            world.reflectionProbeCount = 1u;
            world.reflectionProbes = AllocZeroed<GfxReflectionProbe>(memory, 1uz);
            world.reflectionProbeTextures = AllocZeroed<GfxTexture>(memory, 1uz);
            return CreateDefaultReflectionProbe(world, context, registration, memory, error);
        }

        if (reflectionProbes->data.size() % REFLECTION_PROBE_RECORD_SIZE != 0uz || !FitsUnsigned(reflectionProbes->data.size() / REFLECTION_PROBE_RECORD_SIZE + 1uz))
        {
            error = "reflection-probe lump has funny size";
            return false;
        }

        const auto rawProbeCount = reflectionProbes->data.size() / REFLECTION_PROBE_RECORD_SIZE;
        world.reflectionProbeCount = static_cast<unsigned>(rawProbeCount + 1uz);
        world.reflectionProbes = AllocZeroed<GfxReflectionProbe>(memory, world.reflectionProbeCount);
        // The fastfile DB loader only allocates this runtime array when the
        // serialized pointer is non-null. R_LoadWorld then fills it from each
        // probe image's basemap after the world has been loaded.
        world.reflectionProbeTextures = AllocZeroed<GfxTexture>(memory, world.reflectionProbeCount);

        if (!CreateDefaultReflectionProbe(world, context, registration, memory, error))
            return false;

        for (auto rawProbeIndex = 0uz; rawProbeIndex < rawProbeCount; rawProbeIndex++)
        {
            const auto probeIndex = rawProbeIndex + 1uz;
            const auto* record = reflectionProbes->data.data() + rawProbeIndex * REFLECTION_PROBE_RECORD_SIZE;
            auto& probe = world.reflectionProbes[probeIndex];
            CopyFloat3(record, probe.origin);

            const auto imageName = std::format("*reflection_probe{}", probeIndex);
            auto* image = CreateGeneratedImage(memory,
                                               imageName,
                                               MAPTYPE_CUBE,
                                               TS_COLOR_MAP,
                                               IMG_CATEGORY_AUTO_GENERATED,
                                               REFLECTION_PROBE_SIZE,
                                               REFLECTION_PROBE_SIZE,
                                               1u,
                                               oat::D3DFMT_A8R8G8B8,
                                               static_cast<char>(image::iwi6::IMG_FLAG_CUBEMAP),
                                               nullptr,
                                               REFLECTION_PROBE_RAW_DATA_SIZE);
            CopyTransformedReflectionProbePixels(image->texture.loadDef->data, record + sizeof(float) * 3uz + REFLECTION_PROBE_NAME_SIZE);

            auto* imageInfo = AddGeneratedImage(context, registration, imageName, image);
            if (!imageInfo)
            {
                error = "could not register generated reflection probe image";
                return false;
            }

            probe.reflectionImage = imageInfo->Asset();
        }

        return true;
    }

    [[nodiscard]] int CellForPoint(const GfxWorld& world, const float (&origin)[3])
    {
        if (!world.dpvsPlanes.nodes || !world.dpvsPlanes.planes || world.dpvsPlanes.cellCount <= 0)
            return -1;

        const auto cellCountPlusOne = world.dpvsPlanes.cellCount + 1;
        auto* node = world.dpvsPlanes.nodes;
        while (true)
        {
            const auto cellValue = static_cast<int>(node[0]);
            const auto planeIndex = cellValue - cellCountPlusOne;
            if (planeIndex < 0)
                return cellValue - 1;

            if (planeIndex >= world.planeCount)
                return -1;

            const auto& plane = world.dpvsPlanes.planes[planeIndex];
            const auto dist = origin[0] * plane.normal[0] + origin[1] * plane.normal[1] + origin[2] * plane.normal[2] - plane.dist;
            node += dist <= 0.0f ? node[1] : 2u;
        }
    }

    [[nodiscard]] float DistanceSquared(const float (&left)[3], const float (&right)[3])
    {
        const auto x = left[0] - right[0];
        const auto y = left[1] - right[1];
        const auto z = left[2] - right[2];
        return x * x + y * y + z * z;
    }

    [[nodiscard]] unsigned FindNearestReflectionProbe(const GfxWorld& world, const float (&origin)[3])
    {
        auto bestProbe = 0u;
        auto bestProbeDist = std::numeric_limits<float>::max();

        // Probe 0 is the generated default. The linker only falls back to it
        // when there are no authored probes.
        for (auto probeIndex = 1u; probeIndex < world.reflectionProbeCount; probeIndex++)
        {
            const auto testProbeDist = DistanceSquared(origin, world.reflectionProbes[probeIndex].origin);
            if (testProbeDist < bestProbeDist)
            {
                bestProbeDist = testProbeDist;
                bestProbe = probeIndex;
            }
        }

        return bestProbe;
    }

    [[nodiscard]] unsigned FindNearestReflectionProbeInCell(const GfxWorld& world, const GfxCell& cell, const float (&origin)[3])
    {
        if (cell.reflectionProbeCount <= 0 || !cell.reflectionProbes)
            return FindNearestReflectionProbe(world, origin);

        auto bestProbe = 0u;
        auto bestProbeDist = std::numeric_limits<float>::max();
        for (auto cellProbeIndex = 0; cellProbeIndex < cell.reflectionProbeCount; cellProbeIndex++)
        {
            const auto probeIndex = static_cast<unsigned>(static_cast<unsigned char>(cell.reflectionProbes[cellProbeIndex]));
            if (probeIndex >= world.reflectionProbeCount)
                continue;

            const auto testProbeDist = DistanceSquared(origin, world.reflectionProbes[probeIndex].origin);
            if (testProbeDist < bestProbeDist)
            {
                bestProbeDist = testProbeDist;
                bestProbe = probeIndex;
            }
        }

        return bestProbe;
    }

    [[nodiscard]] unsigned CalcReflectionProbeIndex(const GfxWorld& world, const float (&origin)[3])
    {
        const auto cellIndex = CellForPoint(world, origin);
        if (cellIndex < 0 || cellIndex >= world.dpvsPlanes.cellCount || !world.cells)
            return FindNearestReflectionProbe(world, origin);

        return FindNearestReflectionProbeInCell(world, world.cells[cellIndex], origin);
    }

    void PopulateWorldStaticModelReflectionProbes(GfxWorld& world)
    {
        if (world.reflectionProbeCount == 0u || !world.reflectionProbes || world.dpvs.smodelCount == 0u || !world.dpvs.smodelInsts || !world.dpvs.smodelDrawInsts)
            return;

        for (auto smodelIndex = 0u; smodelIndex < world.dpvs.smodelCount; smodelIndex++)
        {
            const auto& smodelInst = world.dpvs.smodelInsts[smodelIndex];
            float center[3]{
                (smodelInst.mins[0] + smodelInst.maxs[0]) * 0.5f,
                (smodelInst.mins[1] + smodelInst.maxs[1]) * 0.5f,
                (smodelInst.mins[2] + smodelInst.maxs[2]) * 0.5f,
            };

            world.dpvs.smodelDrawInsts[smodelIndex].reflectionProbeIndex = static_cast<char>(CalcReflectionProbeIndex(world, center));
        }
    }

    [[nodiscard]] bool PopulateWorldLightGrid(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* header = bsp.GetLump(LUMP_LIGHTGRID_HEADER);
        if (header)
        {
            if (header->data.size() < 20uz || (header->data.size() - 20uz) % sizeof(uint16_t) != 0uz)
            {
                error = "lightgrid header lump has funny size";
                return false;
            }

            auto& lightGrid = world.lightGrid;
            std::memcpy(lightGrid.mins, header->data.data(), sizeof(lightGrid.mins));
            std::memcpy(lightGrid.maxs, header->data.data() + sizeof(lightGrid.mins), sizeof(lightGrid.maxs));
            lightGrid.rowAxis = ReadU32(header->data.data(), 12uz);
            lightGrid.colAxis = ReadU32(header->data.data(), 16uz);

            const auto rowCount = (header->data.size() - 20uz) / sizeof(uint16_t);
            lightGrid.rowDataStart = rowCount > 0uz ? AllocZeroed<uint16_t>(memory, rowCount) : nullptr;
            if (rowCount > 0uz)
                std::memcpy(lightGrid.rowDataStart, header->data.data() + 20uz, rowCount * sizeof(uint16_t));
        }

        const auto* rawRows = bsp.GetLump(LUMP_LIGHTGRID_ROWS);
        if (rawRows)
        {
            if (!FitsUnsigned(rawRows->data.size()))
            {
                error = "lightgrid raw row lump is too large";
                return false;
            }

            world.lightGrid.rawRowDataSize = static_cast<unsigned>(rawRows->data.size());
            world.lightGrid.rawRowData = AllocCopy<char>(memory, rawRows->data);
        }

        const auto* entries = bsp.GetLump(LUMP_LIGHTGRID_ENTRIES);
        if (entries)
        {
            if (entries->data.size() % RAW_LIGHTGRID_ENTRY_SIZE != 0uz || !FitsUnsigned(RecordCount(*entries, RAW_LIGHTGRID_ENTRY_SIZE)))
            {
                error = "lightgrid entry lump has funny size";
                return false;
            }

            world.lightGrid.entryCount = static_cast<unsigned>(RecordCount(*entries, RAW_LIGHTGRID_ENTRY_SIZE));
            world.lightGrid.entries = AllocCopy<GfxLightGridEntry>(memory, entries->data);
        }

        const auto* colors = bsp.GetLump(LUMP_LIGHTGRID_COLORS);
        if (colors)
        {
            if (colors->data.size() % RAW_LIGHTGRID_COLOR_SIZE != 0uz || !FitsUnsigned(RecordCount(*colors, RAW_LIGHTGRID_COLOR_SIZE) + 1uz))
            {
                error = "lightgrid color lump has funny size";
                return false;
            }

            const auto rawColorCount = RecordCount(*colors, RAW_LIGHTGRID_COLOR_SIZE);
            // The stock linker appends a runtime fallback color set that is not
            // present in the raw BSP. Keep the raw colors first so dumping can
            // drop the synthesized final entry again.
            world.lightGrid.colorCount = static_cast<unsigned>(rawColorCount + 1uz);
            world.lightGrid.colors = AllocZeroed<GfxLightGridColors>(memory, rawColorCount + 1uz);
            if (rawColorCount > 0uz)
                std::memcpy(world.lightGrid.colors, colors->data.data(), rawColorCount * RAW_LIGHTGRID_COLOR_SIZE);
        }

        return true;
    }

    [[nodiscard]] std::optional<size_t>
        FinishWorldAabbTree_r(const GfxWorld& world, GfxAabbTree* trees, const size_t treeIndex, size_t totalTreesUsed, const size_t treeCount, std::string& error)
    {
        auto& tree = trees[treeIndex];
        ClearBounds(tree.mins, tree.maxs);

        if (tree.childCount > 0u)
        {
            const auto childStart = totalTreesUsed;
            const auto childCount = static_cast<size_t>(tree.childCount);
            if (childStart + childCount > treeCount)
            {
                error = std::format("AABB tree {} children extend past tree count", treeIndex);
                return std::nullopt;
            }

            // Raw BSP AABB trees are stored as a flat list. The linker turns
            // each parent into a byte offset to the contiguous child group;
            // runtime mark/DPVS walks use ((char*)tree + childrenOffset).
            tree.childrenOffset = static_cast<int>(reinterpret_cast<const char*>(&trees[childStart]) - reinterpret_cast<const char*>(&tree));

            totalTreesUsed += childCount;
            for (auto childIndex = 0uz; childIndex < childCount; childIndex++)
            {
                const auto childTreeIndex = childStart + childIndex;
                const auto nextTree = FinishWorldAabbTree_r(world, trees, childTreeIndex, totalTreesUsed, treeCount, error);
                if (!nextTree)
                    return std::nullopt;

                totalTreesUsed = *nextTree;
                ExpandBounds(trees[childTreeIndex].mins, trees[childTreeIndex].maxs, tree.mins, tree.maxs);
            }
        }
        else
        {
            const auto startSurface = static_cast<size_t>(tree.startSurfIndex);
            const auto surfaceCount = static_cast<size_t>(tree.surfaceCount);
            if (startSurface + surfaceCount > static_cast<size_t>(world.surfaceCount))
            {
                error = std::format("AABB tree {} surface range is outside world surfaces", treeIndex);
                return std::nullopt;
            }

            for (auto surfaceOffset = 0uz; surfaceOffset < surfaceCount; surfaceOffset++)
                ExpandBounds(world.dpvs.surfaces[startSurface + surfaceOffset].bounds[0],
                             world.dpvs.surfaces[startSurface + surfaceOffset].bounds[1],
                             tree.mins,
                             tree.maxs);
        }

        return totalTreesUsed;
    }

    [[nodiscard]] bool FinishWorldAabbTrees(const GfxWorld& world, GfxAabbTree* trees, const size_t treeCount, std::string& error)
    {
        auto treeIndex = 0uz;
        while (treeIndex < treeCount)
        {
            const auto nextTree = FinishWorldAabbTree_r(world, trees, treeIndex, treeIndex + 1uz, treeCount, error);
            if (!nextTree)
                return false;

            treeIndex = *nextTree;
        }

        return true;
    }

    [[nodiscard]] size_t AabbTreeSubtreeCount(const GfxAabbTree& tree)
    {
        auto count = 1uz;
        const auto* children = reinterpret_cast<const GfxAabbTree*>(reinterpret_cast<const char*>(&tree) + tree.childrenOffset);
        for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
            count += AabbTreeSubtreeCount(children[childIndex]);

        return count;
    }

    [[nodiscard]] GfxAabbTree* AabbTreeChildren(GfxAabbTree& tree)
    {
        return reinterpret_cast<GfxAabbTree*>(reinterpret_cast<char*>(&tree) + tree.childrenOffset);
    }

    [[nodiscard]] const GfxAabbTree* AabbTreeChildren(const GfxAabbTree& tree)
    {
        return reinterpret_cast<const GfxAabbTree*>(reinterpret_cast<const char*>(&tree) + tree.childrenOffset);
    }

    [[nodiscard]] bool SetAabbTreeChildrenOffset(GfxAabbTree& tree, const GfxAabbTree* children, std::string& error)
    {
        const auto offset = reinterpret_cast<const char*>(children) - reinterpret_cast<const char*>(&tree);
        if (offset < static_cast<std::ptrdiff_t>(std::numeric_limits<int>::min())
            || offset > static_cast<std::ptrdiff_t>(std::numeric_limits<int>::max()))
        {
            error = "AABB tree children offset is outside int range";
            return false;
        }

        tree.childrenOffset = static_cast<int>(offset);
        return true;
    }

    [[nodiscard]] GfxAabbTree* BuildWorldAabbTrees(const GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, int& treeCount, std::string& error)
    {
        const auto* aabbTrees = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_AABBTREES, LUMP_SIMPLE_AABBTREES);
        if (!aabbTrees || aabbTrees->data.empty())
        {
            treeCount = world.surfaceCount > 0 ? 1 : 0;
            if (treeCount == 0)
                return nullptr;

            auto* result = AllocZeroed<GfxAabbTree>(memory, 1uz);
            std::memcpy(result->mins, world.mins, sizeof(result->mins));
            std::memcpy(result->maxs, world.maxs, sizeof(result->maxs));
            result->surfaceCount = static_cast<uint16_t>(std::min(world.surfaceCount, static_cast<int>(UINT16_MAX)));
            result->surfaceCountNoDecal = result->surfaceCount;
            return result;
        }

        if (aabbTrees->data.size() % RAW_WORLD_AABB_TREE_SIZE != 0uz || !FitsInt(RecordCount(*aabbTrees, RAW_WORLD_AABB_TREE_SIZE)))
        {
            error = "world AABB tree lump has funny size";
            return nullptr;
        }

        treeCount = static_cast<int>(RecordCount(*aabbTrees, RAW_WORLD_AABB_TREE_SIZE));
        auto* result = AllocZeroed<GfxAabbTree>(memory, treeCount);
        for (auto treeIndex = 0uz; treeIndex < static_cast<size_t>(treeCount); treeIndex++)
        {
            const auto* record = aabbTrees->data.data() + treeIndex * RAW_WORLD_AABB_TREE_SIZE;
            auto& tree = result[treeIndex];
            const auto startSurface = ReadU32(record);
            const auto surfaceCount = ReadU32(record, 4uz);
            const auto childCount = ReadU32(record, 8uz);
            if (startSurface > UINT16_MAX || surfaceCount > UINT16_MAX || childCount > UINT16_MAX)
            {
                error = std::format("AABB tree {} value is out of uint16 range", treeIndex);
                return nullptr;
            }

            tree.startSurfIndex = static_cast<uint16_t>(startSurface);
            tree.surfaceCount = static_cast<uint16_t>(surfaceCount);
            tree.childCount = static_cast<uint16_t>(childCount);
            tree.startSurfIndexNoDecal = tree.startSurfIndex;
            tree.surfaceCountNoDecal = tree.surfaceCount;
        }

        if (!FinishWorldAabbTrees(world, result, static_cast<size_t>(treeCount), error))
            return nullptr;

        return result;
    }

    [[nodiscard]] bool PopulateWorldCells(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        int aabbTreeCount = 0;
        auto* aabbTrees = BuildWorldAabbTrees(world, bsp, memory, aabbTreeCount, error);
        if (!error.empty())
            return false;

        const auto* cells = bsp.GetLump(LUMP_CELLS);
        auto cellCount = cells && !cells->data.empty() ? cells->data.size() / RAW_WORLD_CELL_SIZE : 1uz;
        if (cells && cells->data.size() % RAW_WORLD_CELL_SIZE != 0uz)
        {
            error = "cell lump has funny size";
            return false;
        }

        if (!FitsInt(cellCount))
        {
            error = "too many cell records";
            return false;
        }

        world.dpvsPlanes.cellCount = static_cast<int>(cellCount);
        // Stock R_LoadBsp computes this as a byte count for stack/local cell
        // masks, not as a count of 32-bit words.
        world.cellBitsCount = static_cast<int>(16uz * ((cellCount + 127uz) >> 7uz));
        world.cells = AllocZeroed<GfxCell>(memory, cellCount);

        for (auto cellIndex = 0uz; cellIndex < cellCount; cellIndex++)
        {
            auto& cell = world.cells[cellIndex];
            if (cells && !cells->data.empty())
            {
                const auto* record = cells->data.data() + cellIndex * RAW_WORLD_CELL_SIZE;
                CopyFloat3(record, cell.mins);
                CopyFloat3(record + 12uz, cell.maxs);

                constexpr auto SIMPLE_AABB_TREE_INDEX_OFFSET = 26uz;
                const auto aabbTreeIndex = static_cast<size_t>(ReadU16(record, SIMPLE_AABB_TREE_INDEX_OFFSET));
                if (aabbTreeIndex >= static_cast<size_t>(aabbTreeCount))
                {
                    error = std::format("cell {} references invalid AABB tree {}", cellIndex, aabbTreeIndex);
                    return false;
                }

                // v22 stores both layered and simple AABB roots in each cell.
                // The loader currently imports the simple surface/index lumps,
                // so use the simple root index and serialize the contiguous
                // subtree rooted there, matching linker_pc's per-cell fixup.
                cell.aabbTree = &aabbTrees[aabbTreeIndex];
                cell.aabbTreeCount = static_cast<int>(AabbTreeSubtreeCount(*cell.aabbTree));

                constexpr auto REFLECTION_PROBE_LIST_OFFSET = 44uz;
                cell.reflectionProbeCount = static_cast<char>(std::to_integer<unsigned char>(record[REFLECTION_PROBE_LIST_OFFSET]));
                const auto reflectionProbeCount = std::to_integer<unsigned char>(record[REFLECTION_PROBE_LIST_OFFSET]);
                if (reflectionProbeCount > 0u)
                {
                    cell.reflectionProbes = AllocZeroed<char>(memory, reflectionProbeCount);
                    for (auto probeIndex = 0u; probeIndex < reflectionProbeCount && REFLECTION_PROBE_LIST_OFFSET + 1uz + probeIndex < RAW_WORLD_CELL_SIZE; probeIndex++)
                        cell.reflectionProbes[probeIndex] = static_cast<char>(std::to_integer<unsigned char>(record[REFLECTION_PROBE_LIST_OFFSET + 1uz + probeIndex]));
                }
            }
            else
            {
                std::memcpy(cell.mins, world.mins, sizeof(cell.mins));
                std::memcpy(cell.maxs, world.maxs, sizeof(cell.maxs));
                if (aabbTreeCount > 0)
                {
                    cell.aabbTree = aabbTrees;
                    cell.aabbTreeCount = static_cast<int>(AabbTreeSubtreeCount(*cell.aabbTree));
                }
            }
        }

        return true;
    }

    [[nodiscard]] char PortalPlaneSide(const float value, const char positiveValue)
    {
        return value > 0.0f ? positiveValue : 0;
    }

    [[nodiscard]] bool PopulateWorldPortals(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* portals = bsp.GetLump(LUMP_PORTALS);
        const auto* portalVerts = bsp.GetLump(LUMP_PORTALVERTS);
        const auto* cells = bsp.GetLump(LUMP_CELLS);
        if (!portals || !portalVerts || !cells || !world.cells || !world.dpvsPlanes.planes)
            return true;

        if (portals->data.size() % RAW_WORLD_PORTAL_SIZE != 0uz || portalVerts->data.size() % RAW_VEC3_SIZE != 0uz
            || cells->data.size() % RAW_WORLD_CELL_SIZE != 0uz)
        {
            error = "portal, portal-vertex, or cell lump has funny size";
            return false;
        }

        const auto portalCount = RecordCount(*portals, RAW_WORLD_PORTAL_SIZE);
        const auto portalVertCount = RecordCount(*portalVerts, RAW_VEC3_SIZE);
        const auto cellCount = RecordCount(*cells, RAW_WORLD_CELL_SIZE);
        if (!FitsInt(portalCount) || cellCount > static_cast<size_t>(std::max(world.dpvsPlanes.cellCount, 0)))
        {
            error = "portal or cell count is invalid";
            return false;
        }

        auto* vertexData = portalVertCount > 0uz ? AllocZeroed<vec3_t>(memory, portalVertCount) : nullptr;
        for (auto vertexIndex = 0uz; vertexIndex < portalVertCount; vertexIndex++)
            std::memcpy(vertexData[vertexIndex].v, portalVerts->data.data() + vertexIndex * RAW_VEC3_SIZE, RAW_VEC3_SIZE);

        auto* portalData = portalCount > 0uz ? AllocZeroed<GfxPortal>(memory, portalCount) : nullptr;
        for (auto portalIndex = 0uz; portalIndex < portalCount; portalIndex++)
        {
            const auto* record = portals->data.data() + portalIndex * RAW_WORLD_PORTAL_SIZE;
            const auto planeIndex = static_cast<size_t>(ReadU32(record));
            const auto cellIndex = static_cast<size_t>(ReadU32(record, 4uz));
            const auto firstVertex = static_cast<size_t>(ReadU32(record, 8uz));
            const auto vertexCount = std::to_integer<unsigned>(record[12]);

            if (planeIndex >= static_cast<size_t>(world.planeCount))
            {
                error = std::format("portal {} references invalid plane {}", portalIndex, planeIndex);
                return false;
            }

            if (cellIndex >= static_cast<size_t>(world.dpvsPlanes.cellCount))
            {
                error = std::format("portal {} references invalid cell {}", portalIndex, cellIndex);
                return false;
            }

            if (firstVertex + vertexCount > portalVertCount)
            {
                error = std::format("portal {} vertex range is outside portal vertices", portalIndex);
                return false;
            }

            auto& portal = portalData[portalIndex];
            const auto& plane = world.dpvsPlanes.planes[planeIndex];
            portal.plane.coeffs[0] = plane.normal[0];
            portal.plane.coeffs[1] = plane.normal[1];
            portal.plane.coeffs[2] = plane.normal[2];
            portal.plane.coeffs[3] = -plane.dist;
            portal.plane.side[0] = PortalPlaneSide(portal.plane.coeffs[0], 0xC);
            portal.plane.side[1] = PortalPlaneSide(portal.plane.coeffs[1], 0x10);
            portal.plane.side[2] = PortalPlaneSide(portal.plane.coeffs[2], 0x14);
            portal.cell = &world.cells[cellIndex];
            portal.vertices = vertexData ? &vertexData[firstVertex] : nullptr;
            portal.vertexCount = static_cast<char>(vertexCount);
            PerpendicularVector(plane.normal, portal.hullAxis[0]);
            CrossProduct(plane.normal, portal.hullAxis[0], portal.hullAxis[1]);
        }

        for (auto cellIndex = 0uz; cellIndex < cellCount; cellIndex++)
        {
            const auto* record = cells->data.data() + cellIndex * RAW_WORLD_CELL_SIZE;
            const auto firstPortal = static_cast<size_t>(ReadU32(record, 28uz));
            const auto portalCountForCell = ReadU32(record, 32uz);
            if (firstPortal + portalCountForCell > portalCount)
            {
                error = std::format("cell {} portal range is outside portals", cellIndex);
                return false;
            }

            auto& cell = world.cells[cellIndex];
            cell.portalCount = static_cast<int>(portalCountForCell);
            cell.portals = portalCountForCell > 0u ? &portalData[firstPortal] : nullptr;
        }

        return true;
    }

    [[nodiscard]] bool PopulateWorldModels(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* models = bsp.GetLump(LUMP_MODELS);
        if (!models)
            return true;

        if (models->data.size() % RAW_MODEL_SIZE != 0uz || !FitsInt(RecordCount(*models, RAW_MODEL_SIZE)))
        {
            error = "world model lump has funny size";
            return false;
        }

        world.modelCount = static_cast<int>(RecordCount(*models, RAW_MODEL_SIZE));
        world.models = AllocZeroed<GfxBrushModel>(memory, world.modelCount);

        for (auto modelIndex = 0uz; modelIndex < static_cast<size_t>(world.modelCount); modelIndex++)
        {
            const auto* record = models->data.data() + modelIndex * RAW_MODEL_SIZE;
            auto& model = world.models[modelIndex];
            CopyFloat3(record, model.writable.mins);
            CopyFloat3(record + 12uz, model.writable.maxs);
            std::memcpy(model.bounds[0], model.writable.mins, sizeof(model.bounds[0]));
            std::memcpy(model.bounds[1], model.writable.maxs, sizeof(model.bounds[1]));
            // The raw model record keeps the no-decal surface split separately.
            // Radiant recomputes this from AABB/surface data and asserts it
            // still matches, so preserving it matters for round-tripped BSPs.
            model.startSurfIndex = ReadU16(record, 26uz);
            model.surfaceCountNoDecal = ReadU16(record, 28uz);
            model.surfaceCount = ReadU16(record, 30uz);
        }

        if (world.modelCount > 0)
        {
            std::memcpy(world.mins, world.models[0].bounds[0], sizeof(world.mins));
            std::memcpy(world.maxs, world.models[0].bounds[1], sizeof(world.maxs));

            // The raw BSP loader recomputes the no-decal split while loading,
            // but the stock BSP already carries enough of the split in model0
            // to tell us whether AABB ranges need the decal-safe fallback when
            // dumping again. Without this, imported maps with decal surfaces get
            // dumped as if every static surface belonged to the no-decal range.
            world.dpvs.staticSurfaceCountNoDecal = world.models[0].surfaceCountNoDecal;
        }

        return true;
    }

    [[nodiscard]] uint32_t FloatKeyBits(const float value)
    {
        uint32_t result;
        std::memcpy(&result, &value, sizeof(result));
        if ((result & 0x7fffffffu) == 0u)
            return 0u;

        return result;
    }

    struct TriangleKey
    {
        std::array<uint32_t, 9> values{};

        bool operator==(const TriangleKey& other) const
        {
            return values == other.values;
        }
    };

    struct TriangleKeyHash
    {
        std::size_t operator()(const TriangleKey& key) const
        {
            auto result = 1469598103934665603ull;
            for (const auto value : key.values)
            {
                result ^= value;
                result *= 1099511628211ull;
            }

            return static_cast<std::size_t>(result);
        }
    };

    using DecalTriangleMaterialMap = std::unordered_map<TriangleKey, unsigned, TriangleKeyHash>;

    struct DecalTriangleData
    {
        unsigned firstSurfaceIndex = 0u;
        DecalTriangleMaterialMap minMaterialForTriangle;
        std::vector<std::vector<TriangleKey>> surfaceTriangleKeys;
    };

    [[nodiscard]] std::optional<TriangleKey> BuildTriangleKey(const GfxWorld& world, const GfxSurface& surface, const int baseIndex)
    {
        if (!world.indices || !world.vd.vertices || baseIndex < 0 || baseIndex + 2 >= world.indexCount)
            return std::nullopt;

        std::array<std::array<uint32_t, 3>, 3> points{};
        for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
        {
            const auto vertexIndex = surface.tris.firstVertex + world.indices[baseIndex + static_cast<int>(triVertex)];
            if (vertexIndex < 0 || static_cast<unsigned>(vertexIndex) >= world.vertexCount)
                return std::nullopt;

            const auto* xyz = world.vd.vertices[vertexIndex].xyz;
            for (auto axis = 0uz; axis < 3uz; axis++)
                points[triVertex][axis] = FloatKeyBits(xyz[axis]);
        }

        // Stock R_DoWorldTrisCoincide accepts cyclic shifts of the same
        // winding. Canonicalize only those rotations; reversed winding remains
        // distinct, as in the linker.
        TriangleKey best;
        auto initialized = false;
        for (auto rotation = 0uz; rotation < 3uz; rotation++)
        {
            TriangleKey rotated;
            for (auto triVertex = 0uz; triVertex < 3uz; triVertex++)
            {
                const auto& point = points[(rotation + triVertex) % 3uz];
                for (auto axis = 0uz; axis < 3uz; axis++)
                    rotated.values[triVertex * 3uz + axis] = point[axis];
            }

            if (!initialized || rotated.values < best.values)
            {
                best = rotated;
                initialized = true;
            }
        }

        return best;
    }

    [[nodiscard]] DecalTriangleData BuildDecalTriangleData(const GfxWorld& world, const unsigned modelSurfIndexBegin, const unsigned modelSurfIndexEnd)
    {
        DecalTriangleData result;
        result.firstSurfaceIndex = modelSurfIndexBegin;
        result.surfaceTriangleKeys.resize(modelSurfIndexEnd - modelSurfIndexBegin);

        auto totalTriCount = 0uz;
        for (auto surfIndex = modelSurfIndexBegin; surfIndex < modelSurfIndexEnd; surfIndex++)
            totalTriCount += world.dpvs.surfaces[surfIndex].tris.triCount;

        result.minMaterialForTriangle.reserve(totalTriCount);
        for (auto surfIndex = modelSurfIndexBegin; surfIndex < modelSurfIndexEnd; surfIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfIndex];
            if (!surface.material)
                continue;

            auto& surfaceKeys = result.surfaceTriangleKeys[surfIndex - modelSurfIndexBegin];
            surfaceKeys.reserve(surface.tris.triCount);
            const auto materialSortedIndex = static_cast<unsigned>(surface.material->info.drawSurf.fields.materialSortedIndex);
            for (auto triIter = 0u; triIter < surface.tris.triCount; triIter++)
            {
                const auto triangleKey = BuildTriangleKey(world, surface, surface.tris.baseIndex + static_cast<int>(3u * triIter));
                if (!triangleKey)
                    continue;

                surfaceKeys.emplace_back(*triangleKey);
                auto [entry, inserted] = result.minMaterialForTriangle.emplace(*triangleKey, materialSortedIndex);
                if (!inserted)
                    entry->second = std::min(entry->second, materialSortedIndex);
            }
        }

        return result;
    }

    [[nodiscard]] bool IsSurfaceDecalLayer(const GfxWorld& world, const DecalTriangleData& decalTriangleData, const unsigned surfIndex)
    {
        const auto& surface = world.dpvs.surfaces[surfIndex];
        if (!surface.material || surface.tris.triCount == 0u)
            return false;

        const auto& surfaceKeys = decalTriangleData.surfaceTriangleKeys[surfIndex - decalTriangleData.firstSurfaceIndex];
        if (surfaceKeys.empty())
            return false;

        // linker_pc's R_IsSurfaceDecalLayer loops over triCount, but passes
        // surf->tris.baseIndex to R_DoesTriCoverAnyOtherTri each time. That
        // makes the first triangle decide the whole surface's decal flag.
        const auto materialSortedIndex = static_cast<unsigned>(surface.material->info.drawSurf.fields.materialSortedIndex);
        const auto existingTriangle = decalTriangleData.minMaterialForTriangle.find(surfaceKeys[0]);
        return existingTriangle != decalTriangleData.minMaterialForTriangle.end() && materialSortedIndex > existingTriangle->second;
    }

    [[nodiscard]] bool CompareWorldSurfaces(const GfxSurface& left, const GfxSurface& right)
    {
        const auto leftHasLit = MaterialHasTechnique(left.material, TECHNIQUE_LIT_BEGIN);
        const auto rightHasLit = MaterialHasTechnique(right.material, TECHNIQUE_LIT_BEGIN);
        if (leftHasLit != rightHasLit)
            return leftHasLit > rightHasLit;

        if (!leftHasLit)
        {
            const auto leftHasEmissive = MaterialHasTechnique(left.material, TECHNIQUE_EMISSIVE);
            const auto rightHasEmissive = MaterialHasTechnique(right.material, TECHNIQUE_EMISSIVE);
            if (leftHasEmissive != rightHasEmissive)
                return leftHasEmissive > rightHasEmissive;
        }

        const auto leftPrimarySortKey = left.material ? left.material->info.drawSurf.fields.primarySortKey : 0u;
        const auto rightPrimarySortKey = right.material ? right.material->info.drawSurf.fields.primarySortKey : 0u;
        if (leftPrimarySortKey != rightPrimarySortKey)
            return leftPrimarySortKey < rightPrimarySortKey;

        if (U8(left.primaryLightIndex) != U8(right.primaryLightIndex))
            return U8(left.primaryLightIndex) < U8(right.primaryLightIndex);

        const auto leftMaterialSortedIndex = left.material ? left.material->info.drawSurf.fields.materialSortedIndex : 0u;
        const auto rightMaterialSortedIndex = right.material ? right.material->info.drawSurf.fields.materialSortedIndex : 0u;
        if (leftMaterialSortedIndex != rightMaterialSortedIndex)
            return leftMaterialSortedIndex < rightMaterialSortedIndex;

        if (U8(left.reflectionProbeIndex) != U8(right.reflectionProbeIndex))
            return U8(left.reflectionProbeIndex) < U8(right.reflectionProbeIndex);

        if (U8(left.lightmapIndex) != U8(right.lightmapIndex))
            return U8(left.lightmapIndex) < U8(right.lightmapIndex);

        if (left.tris.firstVertex != right.tris.firstVertex)
            return left.tris.firstVertex < right.tris.firstVertex;

        return left.tris.vertexCount < right.tris.vertexCount;
    }

    void ClassifySortedSurfaceRanges(GfxWorld& world, const unsigned surfaceCount)
    {
        auto surfIndex = 0u;
        world.dpvs.litSurfsBegin = 0u;
        while (surfIndex < surfaceCount)
        {
            const auto* material = world.dpvs.surfaces[surfIndex].material;
            if (!material || !material->techniqueSet || !MaterialHasTechnique(material, TECHNIQUE_LIT_BEGIN) || material->info.sortKey >= 0x18u)
                break;

            surfIndex++;
        }

        world.dpvs.litSurfsEnd = surfIndex;
        world.dpvs.decalSurfsBegin = surfIndex;
        while (surfIndex < surfaceCount)
        {
            const auto* material = world.dpvs.surfaces[surfIndex].material;
            if (!material || !material->techniqueSet || !MaterialHasTechnique(material, TECHNIQUE_LIT_BEGIN))
                break;

            surfIndex++;
        }

        world.dpvs.decalSurfsEnd = surfIndex;
        world.dpvs.emissiveSurfsBegin = surfIndex;
        while (surfIndex < surfaceCount)
        {
            const auto* material = world.dpvs.surfaces[surfIndex].material;
            if (!material || !material->techniqueSet || !MaterialHasTechnique(material, TECHNIQUE_EMISSIVE))
                break;

            surfIndex++;
        }

        world.dpvs.emissiveSurfsEnd = surfIndex;
    }

    [[nodiscard]] bool AppendNoDecalAabbTreeSurfaces(
        const GfxWorld& world, GfxAabbTree& tree, std::vector<uint16_t>& sortedSurfIndex, const unsigned sourceSurfaceCount, unsigned& writeIndex, std::string& error)
    {
        if (writeIndex > UINT16_MAX)
        {
            error = "no-decal AABB tree start surface index is out of uint16 range";
            return false;
        }

        tree.startSurfIndexNoDecal = static_cast<uint16_t>(writeIndex);
        if (tree.childCount > 0u)
        {
            auto* children = reinterpret_cast<GfxAabbTree*>(reinterpret_cast<char*>(&tree) + tree.childrenOffset);
            for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
            {
                if (!AppendNoDecalAabbTreeSurfaces(world, children[childIndex], sortedSurfIndex, sourceSurfaceCount, writeIndex, error))
                    return false;
            }
        }
        else
        {
            const auto firstSurfaceIndex = static_cast<unsigned>(tree.startSurfIndex);
            const auto surfaceCount = static_cast<unsigned>(tree.surfaceCount);
            if (firstSurfaceIndex > sourceSurfaceCount || surfaceCount > sourceSurfaceCount - firstSurfaceIndex)
            {
                error = "AABB tree no-decal source surface range is out of bounds";
                return false;
            }

            for (auto surfaceOffset = 0u; surfaceOffset < surfaceCount; surfaceOffset++)
            {
                const auto surfaceIndex = sortedSurfIndex[firstSurfaceIndex + surfaceOffset];
                if (surfaceIndex >= world.dpvs.staticSurfaceCount)
                {
                    error = "AABB tree no-decal source references invalid surface";
                    return false;
                }

                if ((U8(world.dpvs.surfaces[surfaceIndex].flags) & 2u) != 0u)
                    continue;

                if (writeIndex >= sortedSurfIndex.size())
                {
                    error = "too many no-decal AABB tree surfaces";
                    return false;
                }

                sortedSurfIndex[writeIndex++] = surfaceIndex;
            }
        }

        const auto surfaceCountNoDecal = writeIndex - static_cast<unsigned>(tree.startSurfIndexNoDecal);
        if (surfaceCountNoDecal > UINT16_MAX)
        {
            error = "no-decal AABB tree surface count is out of uint16 range";
            return false;
        }

        tree.surfaceCountNoDecal = static_cast<uint16_t>(surfaceCountNoDecal);
        return true;
    }

    [[nodiscard]] bool BuildNoDecalSubModels(GfxWorld& world, std::vector<uint16_t>& sortedSurfIndex, unsigned& noDecalSurfaceCount, std::string& error)
    {
        if (!world.models || world.modelCount <= 0)
            return true;

        for (auto modelIndex = 0; modelIndex < world.modelCount; modelIndex++)
        {
            auto& model = world.models[modelIndex];
            model.surfaceCountNoDecal = 0;
            if (model.surfaceCount == 0u)
                continue;

            const auto begin = static_cast<unsigned>(model.startSurfIndex);
            const auto end = begin + static_cast<unsigned>(model.surfaceCount);
            if (end > static_cast<unsigned>(world.surfaceCount))
            {
                error = std::format("world model {} surface range is out of bounds", modelIndex);
                return false;
            }

            const auto decalTriangleData = BuildDecalTriangleData(world, begin, end);
            for (auto surfIndex = begin; surfIndex < end; surfIndex++)
            {
                auto& surface = world.dpvs.surfaces[surfIndex];
                surface.flags = static_cast<char>(U8(surface.flags) & ~2u);
                if (IsSurfaceDecalLayer(world, decalTriangleData, surfIndex))
                    surface.flags = static_cast<char>(U8(surface.flags) | 2u);
                else
                    model.surfaceCountNoDecal++;
            }
        }

        const auto& rootModel = world.models[0];
        const auto rootSurfaceCount = static_cast<unsigned>(rootModel.surfaceCount);
        auto writeIndex = rootSurfaceCount;
        if (world.dpvsPlanes.cellCount > 0 && world.cells)
        {
            // linker_pc appends the no-decal duplicate surface range by walking
            // each cell's AABB tree, not by scanning model0 linearly. The same
            // pass also rewrites startSurfIndexNoDecal/surfaceCountNoDecal on
            // every tree node for runtime DPVS traversal.
            for (auto cellIndex = 0; cellIndex < world.dpvsPlanes.cellCount; cellIndex++)
            {
                auto& cell = world.cells[cellIndex];
                if (!cell.aabbTree)
                    continue;

                if (!AppendNoDecalAabbTreeSurfaces(world, *cell.aabbTree, sortedSurfIndex, rootSurfaceCount, writeIndex, error))
                    return false;
            }
        }
        else
        {
            for (auto originalSurfIndex = 0u; originalSurfIndex < rootSurfaceCount; originalSurfIndex++)
            {
                const auto sortedIndex = sortedSurfIndex[originalSurfIndex];
                if ((U8(world.dpvs.surfaces[sortedIndex].flags) & 2u) == 0u)
                    sortedSurfIndex[writeIndex++] = sortedIndex;
            }
        }

        noDecalSurfaceCount = writeIndex - rootSurfaceCount;
        return true;
    }

    [[nodiscard]] bool PopulateWorldSurfaceOrganization(GfxWorld& world, MemoryManager& memory, std::string& error)
    {
        if (!world.models || world.modelCount <= 0 || !world.dpvs.surfaces)
            return true;

        auto& rootModel = world.models[0];
        if (rootModel.surfaceCount == 0u)
            return true;

        if (rootModel.startSurfIndex != 0u)
        {
            error = "root world model does not start at surface 0";
            return false;
        }

        const auto surfaceCount = static_cast<unsigned>(rootModel.surfaceCount);
        if (surfaceCount > static_cast<unsigned>(world.surfaceCount) || !FitsUint16(surfaceCount * 2uz))
        {
            error = "root world model surface count is invalid";
            return false;
        }

        std::vector<uint16_t> sortedSurfIndex(static_cast<size_t>(surfaceCount) * 2uz);
        for (auto surfIndex = 0u; surfIndex < surfaceCount; surfIndex++)
        {
            auto& surface = world.dpvs.surfaces[surfIndex];
            sortedSurfIndex[surfIndex] = surface.tris.vertexCount;
            surface.tris.vertexCount = static_cast<uint16_t>(surfIndex);
        }

        std::sort(&world.dpvs.surfaces[0], &world.dpvs.surfaces[surfaceCount], CompareWorldSurfaces);

        for (auto surfIndex = 0u; surfIndex < surfaceCount; surfIndex++)
        {
            auto& surface = world.dpvs.surfaces[surfIndex];
            const auto originalSurfIndex = static_cast<unsigned>(surface.tris.vertexCount);
            if (originalSurfIndex >= surfaceCount)
            {
                error = "surface sort produced an invalid original surface index";
                return false;
            }

            surface.tris.vertexCount = sortedSurfIndex[originalSurfIndex];
            sortedSurfIndex[originalSurfIndex] = static_cast<uint16_t>(surfIndex);
        }

        ClassifySortedSurfaceRanges(world, surfaceCount);

        unsigned noDecalSurfaceCount = 0u;
        if (!BuildNoDecalSubModels(world, sortedSurfIndex, noDecalSurfaceCount, error))
            return false;

        const auto finalSortedCount = static_cast<size_t>(surfaceCount) + noDecalSurfaceCount;
        world.dpvs.sortedSurfIndex = AllocZeroed<uint16_t>(memory, finalSortedCount);
        std::memcpy(world.dpvs.sortedSurfIndex, sortedSurfIndex.data(), finalSortedCount * sizeof(uint16_t));
        world.dpvs.staticSurfaceCount = surfaceCount;
        world.dpvs.staticSurfaceCountNoDecal = noDecalSurfaceCount;
        rootModel.surfaceCountNoDecal = static_cast<uint16_t>(noDecalSurfaceCount);
        return true;
    }

    void ParseGfxLightRecord(const std::byte* record, GfxLight& light)
    {
        light = {};
        light.type = ReadRawByte(record, RAW_LIGHT_TYPE_OFFSET);
        light.canUseShadowMap = ReadRawByte(record, RAW_LIGHT_CAN_USE_SHADOW_MAP_OFFSET);
        light.unused[0] = ReadRawByte(record, RAW_LIGHT_UNUSED_OFFSET);
        light.unused[1] = ReadRawByte(record, RAW_LIGHT_UNUSED_OFFSET + 1uz);
        CopyFloat3(record + RAW_LIGHT_COLOR_OFFSET, light.color);
        CopyFloat3(record + RAW_LIGHT_DIR_OFFSET, light.dir);
        CopyFloat3(record + RAW_LIGHT_ORIGIN_OFFSET, light.origin);
        CopyUnaligned(record + RAW_LIGHT_RADIUS_OFFSET, light.radius);
        CopyUnaligned(record + RAW_LIGHT_COS_HALF_FOV_OUTER_OFFSET, light.cosHalfFovOuter);
        CopyUnaligned(record + RAW_LIGHT_COS_HALF_FOV_INNER_OFFSET, light.cosHalfFovInner);
        light.exponent = ReadI32(record, RAW_LIGHT_EXPONENT_OFFSET);
    }

    [[nodiscard]] bool InferWorldPrimaryLightCount(const IW3::d3dbsp::File& bsp, const size_t rawPrimaryLightCount, unsigned& primaryLightCount, std::string& error)
    {
        const auto* regionCounts = bsp.GetLump(LUMP_LIGHT_REGION_COUNTS);
        if (regionCounts && !regionCounts->data.empty())
        {
            if (!FitsUnsigned(regionCounts->data.size()))
            {
                error = "light-region count lump is too large";
                return false;
            }

            // The primary-light lump is ComWorld data. GfxWorld primary-light
            // arrays are sized by the light-region count lump when it exists.
            primaryLightCount = static_cast<unsigned>(regionCounts->data.size());
            return true;
        }

        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        auto maxPrimaryLightIndex = 0u;
        auto foundSurface = false;

        if (surfaces && !surfaces->data.empty())
        {
            if (surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz)
            {
                error = "world surface lump has funny size";
                return false;
            }

            const auto surfaceCount = RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE);
            for (auto surfaceIndex = 0uz; surfaceIndex < surfaceCount; surfaceIndex++)
            {
                const auto* record = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
                maxPrimaryLightIndex = std::max(maxPrimaryLightIndex, std::to_integer<unsigned>(record[4]));
                foundSurface = true;
            }
        }

        primaryLightCount = foundSurface ? maxPrimaryLightIndex + 1u : static_cast<unsigned>(std::min(rawPrimaryLightCount, static_cast<size_t>(UINT32_MAX)));
        return true;
    }

    [[nodiscard]] bool PopulateWorldPrimaryLights(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* primaryLights = bsp.GetLump(LUMP_PRIMARY_LIGHTS);
        const auto size = primaryLights ? primaryLights->data.size() : 0uz;
        if (size % IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE != 0uz || !FitsUnsigned(size / IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE))
        {
            error = "primary-light lump has funny size";
            return false;
        }

        const auto rawPrimaryLightCount = size / IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE;
        if (!InferWorldPrimaryLightCount(bsp, rawPrimaryLightCount, world.primaryLightCount, error))
            return false;

        if (world.primaryLightCount > rawPrimaryLightCount)
        {
            error = std::format("GfxWorld primary light count {} exceeds raw primary-light record count {}", world.primaryLightCount, rawPrimaryLightCount);
            return false;
        }

        if (rawPrimaryLightCount == 0uz)
            return true;

        // Stock v22 maps normally reserve primary light 0 as "none" and store
        // the sun at index 1. The stock loader uses that exact convention
        // instead of scanning for any sun record.
        world.sunPrimaryLightIndex = 0u;
        if (rawPrimaryLightCount > 1uz)
        {
            const auto* candidateSunRecord = primaryLights->data.data() + IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE;
            if (ReadRawByte(candidateSunRecord, RAW_LIGHT_TYPE_OFFSET) == 1)
                world.sunPrimaryLightIndex = 1u;
        }

        world.sunLight = AllocZeroed<GfxLight>(memory);
        ParseGfxLightRecord(primaryLights->data.data() + static_cast<size_t>(world.sunPrimaryLightIndex) * IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE, *world.sunLight);
        std::memcpy(world.sunColorFromBsp, world.sunLight->color, sizeof(world.sunColorFromBsp));
        world.lightGrid.sunPrimaryLightIndex = world.sunPrimaryLightIndex;
        return true;
    }

    [[nodiscard]] bool PopulateWorldLightRegions(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        if (world.primaryLightCount == 0u)
            return true;

        const auto* counts = bsp.GetLump(LUMP_LIGHT_REGION_COUNTS);
        if (!counts || counts->data.empty())
            return true;

        world.lightGrid.hasLightRegions = true;
        world.lightRegion = AllocZeroed<GfxLightRegion>(memory, world.primaryLightCount);

        const auto* hulls = bsp.GetLump(LUMP_LIGHT_REGION_HULLS);
        const auto* axes = bsp.GetLump(LUMP_LIGHT_REGION_AXES);
        auto hullOffset = 0uz;
        auto axisOffset = 0uz;

        for (auto lightIndex = 0uz; lightIndex < world.primaryLightCount && lightIndex < counts->data.size(); lightIndex++)
        {
            const auto hullCount = std::to_integer<unsigned char>(counts->data[lightIndex]);
            if (hullCount == 0u)
                continue;

            if (!hulls || hullOffset + static_cast<size_t>(hullCount) * RAW_LIGHT_REGION_HULL_SIZE > hulls->data.size())
            {
                error = "light-region hull lump is truncated";
                return false;
            }

            auto& region = world.lightRegion[lightIndex];
            region.hullCount = hullCount;
            region.hulls = AllocZeroed<GfxLightRegionHull>(memory, hullCount);

            for (auto hullIndex = 0uz; hullIndex < hullCount; hullIndex++)
            {
                const auto* hullRecord = hulls->data.data() + hullOffset;
                auto& hull = region.hulls[hullIndex];
                std::memcpy(hull.kdopMidPoint, hullRecord, sizeof(hull.kdopMidPoint));
                std::memcpy(hull.kdopHalfSize, hullRecord + sizeof(hull.kdopMidPoint), sizeof(hull.kdopHalfSize));
                hull.axisCount = ReadU32(hullRecord, 72uz);
                hullOffset += RAW_LIGHT_REGION_HULL_SIZE;

                if (hull.axisCount == 0u)
                    continue;

                if (!axes || axisOffset + static_cast<size_t>(hull.axisCount) * RAW_LIGHT_REGION_AXIS_SIZE > axes->data.size())
                {
                    error = "light-region axis lump is truncated";
                    return false;
                }

                hull.axis = AllocZeroed<GfxLightRegionAxis>(memory, hull.axisCount);
                std::memcpy(hull.axis, axes->data.data() + axisOffset, static_cast<size_t>(hull.axisCount) * RAW_LIGHT_REGION_AXIS_SIZE);
                axisOffset += static_cast<size_t>(hull.axisCount) * RAW_LIGHT_REGION_AXIS_SIZE;
            }
        }

        return true;
    }

    [[nodiscard]] bool PopulateWorldShadowGeometry(GfxWorld& world, MemoryManager& memory)
    {
        if (world.primaryLightCount == 0u || world.shadowGeom)
            return true;

        // R_AllocShadowGeometryHeaderMemory creates one zeroed header per
        // primary light while loading raw BSPs. Fastfile loading expects that
        // array to already exist before R_SetPrimaryLightShadowSurfaces clears
        // and rebuilds the surface counts at runtime.
        world.shadowGeom = AllocZeroed<GfxShadowGeometry>(memory, world.primaryLightCount);
        return true;
    }

    void PopulateStaticModelGroundLighting(const EntityBlock& block, GfxStaticModelInst& inst, GfxStaticModelDrawInst& drawInst)
    {
        const auto gndLt = EntityField(block, "gndLt");
        if (gndLt.size() < 10uz)
            return;

        const auto b = ParseHexByte(gndLt, 0uz);
        const auto g = ParseHexByte(gndLt, 2uz);
        const auto r = ParseHexByte(gndLt, 4uz);
        const auto a = ParseHexByte(gndLt, 6uz);
        const auto primaryLightIndex = ParseHexByte(gndLt, 8uz);
        if (!b || !g || !r || !a || !primaryLightIndex)
            return;

        drawInst.primaryLightIndex = static_cast<char>(*primaryLightIndex);

        // linker_pc only preserves the parsed misc_model ground-light color
        // when the model has XModel::flags bit 0 set and the color is non-zero.
        // In the drop path it recomputes primaryLightIndex from the model
        // bounds/light regions; keep the parsed index until that lookup exists.
        if (!drawInst.model || (static_cast<unsigned char>(drawInst.model->flags) & 1u) == 0u || (*r == 0u && *g == 0u && *b == 0u && *a == 0u))
            return;

        // The linker parses gndLt as B,G,R,A,primaryLightIndex. Runtime
        // GfxColor is stored in R,G,B,A byte order.
        inst.groundLighting.array[0] = static_cast<char>(*r);
        inst.groundLighting.array[1] = static_cast<char>(*g);
        inst.groundLighting.array[2] = static_cast<char>(*b);
        inst.groundLighting.array[3] = static_cast<char>(*a);
    }

    [[nodiscard]] bool PopulateWorldStaticModels(
        GfxWorld& world,
        const clipMap_t* clipMap,
        const std::vector<const EntityBlock*>& staticModelBlocks,
        const std::vector<XAssetInfo<XModel>*>& staticModelDependencies,
        MemoryManager& memory,
        std::string& error)
    {
        std::vector<std::pair<const EntityBlock*, XModel*>> validStaticModels;
        validStaticModels.reserve(staticModelBlocks.size());
        for (auto i = 0uz; i < staticModelBlocks.size(); i++)
        {
            if (i < staticModelDependencies.size() && staticModelDependencies[i])
                validStaticModels.emplace_back(staticModelBlocks[i], staticModelDependencies[i]->Asset());
        }

        if (!FitsUnsigned(validStaticModels.size()))
        {
            error = "too many static model records";
            return false;
        }

        world.dpvs.smodelCount = static_cast<unsigned>(validStaticModels.size());
        if (validStaticModels.empty())
            return true;

        world.dpvs.smodelDrawInsts = AllocZeroed<GfxStaticModelDrawInst>(memory, validStaticModels.size());
        world.dpvs.smodelInsts = AllocZeroed<GfxStaticModelInst>(memory, validStaticModels.size());

        for (auto modelIndex = 0uz; modelIndex < validStaticModels.size(); modelIndex++)
        {
            const auto& [block, model] = validStaticModels[modelIndex];
            auto& drawInst = world.dpvs.smodelDrawInsts[modelIndex];
            auto& inst = world.dpvs.smodelInsts[modelIndex];
            const auto origin = ParseFloat3(EntityField(*block, "origin")).value_or(std::array<float, 3>{});
            const auto angles = ParseFloat3(EntityField(*block, "angles")).value_or(std::array<float, 3>{});
            const auto scale = StaticModelScale(*block);

            float axis[3][3]{};
            AnglesToAxis(angles, axis);

            drawInst.cullDist = std::numeric_limits<float>::max();
            drawInst.model = model;
            drawInst.placement.scale = scale;
            std::copy(origin.begin(), origin.end(), drawInst.placement.origin);
            std::memcpy(drawInst.placement.axis, axis, sizeof(drawInst.placement.axis));
            // Static model cache slots are runtime state. Zero means uncached;
            // R_CacheStaticModelSurface allocates a cache entry on demand.
            // 0xffff is SMODEL_INDEX_NONE for cache leaves, not for draw insts.

            if ((ParseInt(EntityField(*block, "spawnflags")) & 2) != 0)
                drawInst.flags = 1;

            if (clipMap && clipMap->staticModelList && modelIndex < clipMap->numStaticModels)
            {
                std::memcpy(inst.mins, clipMap->staticModelList[modelIndex].absmin, sizeof(inst.mins));
                std::memcpy(inst.maxs, clipMap->staticModelList[modelIndex].absmax, sizeof(inst.maxs));
            }
            else if (model)
            {
                float mins[3]{};
                float maxs[3]{};
                for (auto axisIndex = 0uz; axisIndex < 3uz; axisIndex++)
                {
                    mins[axisIndex] = std::numeric_limits<float>::max();
                    maxs[axisIndex] = std::numeric_limits<float>::lowest();
                }

                for (auto corner = 0u; corner < 8u; corner++)
                {
                    const auto x = (corner & 1u) != 0u ? model->maxs.x : model->mins.x;
                    const auto y = (corner & 2u) != 0u ? model->maxs.y : model->mins.y;
                    const auto z = (corner & 4u) != 0u ? model->maxs.z : model->mins.z;
                    float transformed[3]{};
                    TransformStaticModelPoint(axis, origin, scale, x, y, z, transformed);
                    for (auto axisIndex = 0uz; axisIndex < 3uz; axisIndex++)
                    {
                        mins[axisIndex] = std::min(mins[axisIndex], transformed[axisIndex]);
                        maxs[axisIndex] = std::max(maxs[axisIndex], transformed[axisIndex]);
                    }
                }

                std::memcpy(inst.mins, mins, sizeof(inst.mins));
                std::memcpy(inst.maxs, maxs, sizeof(inst.maxs));
            }

            PopulateStaticModelGroundLighting(*block, inst, drawInst);
        }

        return true;
    }

    [[nodiscard]] bool BoundsContain(const float (&outerMins)[3], const float (&outerMaxs)[3], const float (&innerMins)[3], const float (&innerMaxs)[3])
    {
        return outerMins[0] <= innerMins[0] && outerMins[1] <= innerMins[1] && outerMins[2] <= innerMins[2] && outerMaxs[0] >= innerMaxs[0]
               && outerMaxs[1] >= innerMaxs[1] && outerMaxs[2] >= innerMaxs[2];
    }

    [[nodiscard]] int BoxOnPlaneSide(const float (&mins)[3], const float (&maxs)[3], const cplane_s& plane)
    {
        const auto dist = plane.dist;
        const auto planeType = static_cast<unsigned char>(plane.type);
        if (planeType < 3u)
        {
            if (mins[planeType] >= dist)
                return 1;
            if (maxs[planeType] <= dist)
                return 2;

            return 3;
        }

        auto minDist = 0.0f;
        auto maxDist = 0.0f;
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            const auto normal = plane.normal[axis];
            if (normal >= 0.0f)
            {
                minDist += normal * mins[axis];
                maxDist += normal * maxs[axis];
            }
            else
            {
                minDist += normal * maxs[axis];
                maxDist += normal * mins[axis];
            }
        }

        if (minDist >= dist)
            return 1;
        if (maxDist <= dist)
            return 2;

        return 3;
    }

    using StaticModelIndexLists = std::unordered_map<GfxAabbTree*, std::vector<uint16_t>>;

    void AddStaticModelToTreeList(StaticModelIndexLists& staticModelIndexesByTree, GfxAabbTree& tree, const uint16_t staticModelIndex)
    {
        auto& indexes = staticModelIndexesByTree[&tree];
        if (indexes.empty() || indexes.back() != staticModelIndex)
            indexes.emplace_back(staticModelIndex);
    }

    void MoveStaticModelTreeList(StaticModelIndexLists& staticModelIndexesByTree, GfxAabbTree& oldTree, GfxAabbTree& newTree)
    {
        auto existing = staticModelIndexesByTree.find(&oldTree);
        if (existing == staticModelIndexesByTree.end())
            return;

        auto indexes = std::move(existing->second);
        staticModelIndexesByTree.erase(existing);
        auto& targetIndexes = staticModelIndexesByTree[&newTree];
        targetIndexes.insert(targetIndexes.end(), std::make_move_iterator(indexes.begin()), std::make_move_iterator(indexes.end()));
    }

    [[nodiscard]] bool CopyAabbTreeToNewAddress(
        StaticModelIndexLists& staticModelIndexesByTree, GfxAabbTree& oldTree, GfxAabbTree& newTree, std::string& error)
    {
        newTree = oldTree;
        MoveStaticModelTreeList(staticModelIndexesByTree, oldTree, newTree);

        if (oldTree.childCount > 0u)
            return SetAabbTreeChildrenOffset(newTree, AabbTreeChildren(oldTree), error);

        newTree.childrenOffset = 0;
        return true;
    }

    [[nodiscard]] bool AppendStaticModelOnlyChild(
        StaticModelIndexLists& staticModelIndexesByTree, GfxAabbTree& tree, const GfxStaticModelInst& smodelInst, MemoryManager& memory, std::string& error)
    {
        if (tree.childCount == std::numeric_limits<uint16_t>::max())
        {
            error = "too many AABB tree children";
            return false;
        }

        const auto oldChildCount = tree.childCount;
        auto* oldChildren = AabbTreeChildren(tree);
        auto* newChildren = AllocZeroed<GfxAabbTree>(memory, static_cast<size_t>(oldChildCount) + 1uz);
        for (auto childIndex = 0u; childIndex < oldChildCount; childIndex++)
        {
            if (!CopyAabbTreeToNewAddress(staticModelIndexesByTree, oldChildren[childIndex], newChildren[childIndex], error))
                return false;
        }

        if (!SetAabbTreeChildrenOffset(tree, newChildren, error))
            return false;

        auto& newChild = newChildren[oldChildCount];
        std::memcpy(newChild.mins, smodelInst.mins, sizeof(newChild.mins));
        std::memcpy(newChild.maxs, smodelInst.maxs, sizeof(newChild.maxs));
        tree.childCount = static_cast<uint16_t>(oldChildCount + 1u);
        return true;
    }

    [[nodiscard]] bool AddStaticModelToAabbTree_r(
        const GfxWorld& world,
        StaticModelIndexLists& staticModelIndexesByTree,
        GfxAabbTree& tree,
        const uint16_t staticModelIndex,
        MemoryManager& memory,
        std::string& error)
    {
        AddStaticModelToTreeList(staticModelIndexesByTree, tree, staticModelIndex);

        if (tree.childCount == 0u || tree.childrenOffset == 0)
            return true;

        const auto& smodelInst = world.dpvs.smodelInsts[staticModelIndex];
        auto* children = AabbTreeChildren(tree);

        // linker_pc descends through the one child that fully contains the
        // static model bounds. If none contains it, it reuses an existing
        // static-model-only child or appends a new one; it does not fan out to
        // all overlapping children. This tree mutation is required for the
        // SIMPLE_AABBTREES lump to reach the same fixed point as linker_pc.
        for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
        {
            auto& child = children[childIndex];
            if (BoundsContain(child.mins, child.maxs, smodelInst.mins, smodelInst.maxs))
            {
                return AddStaticModelToAabbTree_r(world, staticModelIndexesByTree, child, staticModelIndex, memory, error);
            }
        }

        for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
        {
            auto& child = children[childIndex];
            if (child.surfaceCount == 0u)
            {
                ExpandBounds(smodelInst.mins, smodelInst.maxs, child.mins, child.maxs);
                return AddStaticModelToAabbTree_r(world, staticModelIndexesByTree, child, staticModelIndex, memory, error);
            }
        }

        if (!AppendStaticModelOnlyChild(staticModelIndexesByTree, tree, smodelInst, memory, error))
            return false;

        children = AabbTreeChildren(tree);
        return AddStaticModelToAabbTree_r(world, staticModelIndexesByTree, children[tree.childCount - 1u], staticModelIndex, memory, error);
    }

    [[nodiscard]] bool AddStaticModelToCell(
        const GfxWorld& world,
        StaticModelIndexLists& staticModelIndexesByTree,
        const uint16_t staticModelIndex,
        const int cellIndex,
        MemoryManager& memory,
        std::string& error)
    {
        if (cellIndex < 0 || cellIndex >= world.dpvsPlanes.cellCount || !world.cells)
        {
            error = "static model references invalid cell";
            return false;
        }

        auto& cell = world.cells[cellIndex];
        if (!cell.aabbTree)
            return true;

        const auto existing = staticModelIndexesByTree.find(cell.aabbTree);
        if (existing != staticModelIndexesByTree.end() && !existing->second.empty() && existing->second.back() == staticModelIndex)
            return true;

        return AddStaticModelToAabbTree_r(world, staticModelIndexesByTree, *cell.aabbTree, staticModelIndex, memory, error);
    }

    [[nodiscard]] bool FilterStaticModelIntoCells_r(
        const GfxWorld& world,
        StaticModelIndexLists& staticModelIndexesByTree,
        const uint16_t staticModelIndex,
        const uint16_t* node,
        const float (&mins)[3],
        const float (&maxs)[3],
        MemoryManager& memory,
        std::string& error)
    {
        while (true)
        {
            if (!node)
            {
                error = "world node stream is missing";
                return false;
            }

            const auto cellValue = static_cast<int>(node[0]);
            const auto planeIndex = cellValue - world.dpvsPlanes.cellCount - 1;
            if (planeIndex < 0)
                return cellValue != 0 ? AddStaticModelToCell(world, staticModelIndexesByTree, staticModelIndex, cellValue - 1, memory, error) : true;

            if (planeIndex >= world.planeCount || !world.dpvsPlanes.planes)
            {
                error = "world node stream references invalid plane";
                return false;
            }

            const auto& plane = world.dpvsPlanes.planes[planeIndex];
            const auto boxSide = BoxOnPlaneSide(mins, maxs, plane);
            if (boxSide == 3)
            {
                const auto* rightNode = node + node[1];
                const auto planeType = static_cast<unsigned char>(plane.type);
                if (planeType >= 3u)
                {
                    if (!FilterStaticModelIntoCells_r(world, staticModelIndexesByTree, staticModelIndex, node + 2, mins, maxs, memory, error))
                        return false;
                }
                else
                {
                    float frontMins[3]{mins[0], mins[1], mins[2]};
                    float backMaxs[3]{maxs[0], maxs[1], maxs[2]};
                    frontMins[planeType] = plane.dist;
                    backMaxs[planeType] = plane.dist;

                    if (maxs[planeType] > plane.dist
                        && !FilterStaticModelIntoCells_r(world, staticModelIndexesByTree, staticModelIndex, node + 2, frontMins, maxs, memory, error))
                    {
                        return false;
                    }

                    return FilterStaticModelIntoCells_r(world, staticModelIndexesByTree, staticModelIndex, rightNode, mins, backMaxs, memory, error);
                }

                node = rightNode;
                continue;
            }

            if (boxSide == 1)
            {
                node += 2;
                continue;
            }

            if (boxSide == 2)
            {
                node += node[1];
                continue;
            }

            error = "world node plane-side classification failed";
            return false;
        }
    }

    [[nodiscard]] bool CommitStaticModelAabbTreeIndexes(StaticModelIndexLists& staticModelIndexesByTree, MemoryManager& memory, std::string& error)
    {
        for (auto& [tree, indexes] : staticModelIndexesByTree)
        {
            std::sort(indexes.begin(), indexes.end());

            if (!FitsUint16(indexes.size()))
            {
                error = "too many static model indexes in world AABB tree";
                return false;
            }

            tree->smodelIndexCount = static_cast<uint16_t>(indexes.size());
            if (!indexes.empty())
            {
                tree->smodelIndexes = AllocZeroed<uint16_t>(memory, indexes.size());
                std::copy(indexes.begin(), indexes.end(), tree->smodelIndexes);
            }
        }

        return true;
    }

    [[nodiscard]] unsigned SortGfxAabbTreeChildren(
        const GfxWorld& world, const float (&mins)[3], const float (&maxs)[3], uint16_t* staticModels, const unsigned staticModelCount)
    {
        auto childCount = 0u;
        for (auto staticModelOffset = 0u; staticModelOffset < staticModelCount; staticModelOffset++)
        {
            const auto staticModelIndex = staticModels[staticModelOffset];
            const auto& smodelInst = world.dpvs.smodelInsts[staticModelIndex];
            if (!BoundsContain(mins, maxs, smodelInst.mins, smodelInst.maxs))
                continue;

            std::swap(staticModels[childCount], staticModels[staticModelOffset]);
            childCount++;
        }

        return childCount < 2u ? 0u : childCount;
    }

    [[nodiscard]] bool AddSortedStaticModelChild(
        GfxAabbTree& tree, uint16_t*& smodelIndexes, unsigned& remainingModelCount, const unsigned childModelCount, std::string& error)
    {
        if (childModelCount == 0u)
            return true;

        if (tree.childCount == std::numeric_limits<uint16_t>::max())
        {
            error = "too many sorted AABB tree children";
            return false;
        }

        auto* children = AabbTreeChildren(tree);
        auto& childTree = children[tree.childCount++];
        childTree.smodelIndexCount = static_cast<uint16_t>(childModelCount);
        childTree.smodelIndexes = smodelIndexes;
        smodelIndexes += childModelCount;
        remainingModelCount -= childModelCount;
        return true;
    }

    [[nodiscard]] bool SortGfxAabbTree(const GfxWorld& world, GfxAabbTree& tree, MemoryManager& memory, std::string& error)
    {
        if (tree.smodelIndexCount > 1u)
            std::sort(tree.smodelIndexes, tree.smodelIndexes + tree.smodelIndexCount);

        if (tree.childCount > 0u)
        {
            auto* children = AabbTreeChildren(tree);
            for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
            {
                if (!SortGfxAabbTree(world, children[childIndex], memory, error))
                    return false;
            }

            return true;
        }

        if (tree.smodelIndexCount == 0u)
            return true;

        float mins[3]{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        float maxs[3]{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
        for (auto smodelOffset = 0u; smodelOffset < tree.smodelIndexCount; smodelOffset++)
        {
            const auto& smodelInst = world.dpvs.smodelInsts[tree.smodelIndexes[smodelOffset]];
            ExpandBounds(smodelInst.mins, smodelInst.maxs, mins, maxs);
        }

        if (tree.surfaceCount == 0u)
        {
            std::memcpy(tree.mins, mins, sizeof(tree.mins));
            std::memcpy(tree.maxs, maxs, sizeof(tree.maxs));
        }

        if (tree.smodelIndexCount < 8u)
            return true;

        const float middle[3]{(mins[0] + maxs[0]) * 0.5f, (mins[1] + maxs[1]) * 0.5f, (mins[2] + maxs[2]) * 0.5f};
        auto* smodelIndexes = tree.smodelIndexes;
        auto remainingModelCount = static_cast<unsigned>(tree.smodelIndexCount);
        unsigned childModelCounts[4]{};

        float childMins[3]{mins[0], mins[1], mins[2]};
        float childMaxs[3]{middle[0], maxs[1], maxs[2]};
        childModelCounts[0] = SortGfxAabbTreeChildren(world, childMins, childMaxs, smodelIndexes, remainingModelCount);
        smodelIndexes += childModelCounts[0];
        remainingModelCount -= childModelCounts[0];

        childMins[0] = middle[0];
        childMins[1] = mins[1];
        childMins[2] = mins[2];
        childMaxs[0] = maxs[0];
        childMaxs[1] = maxs[1];
        childMaxs[2] = maxs[2];
        childModelCounts[1] = SortGfxAabbTreeChildren(world, childMins, childMaxs, smodelIndexes, remainingModelCount);
        smodelIndexes += childModelCounts[1];
        remainingModelCount -= childModelCounts[1];

        childMins[0] = mins[0];
        childMins[1] = mins[1];
        childMins[2] = mins[2];
        childMaxs[0] = maxs[0];
        childMaxs[1] = middle[1];
        childMaxs[2] = maxs[2];
        childModelCounts[2] = SortGfxAabbTreeChildren(world, childMins, childMaxs, smodelIndexes, remainingModelCount);
        smodelIndexes += childModelCounts[2];
        remainingModelCount -= childModelCounts[2];

        childMins[0] = mins[0];
        childMins[1] = middle[1];
        childMins[2] = mins[2];
        childMaxs[0] = maxs[0];
        childMaxs[1] = maxs[1];
        childMaxs[2] = maxs[2];
        childModelCounts[3] = SortGfxAabbTreeChildren(world, childMins, childMaxs, smodelIndexes, remainingModelCount);
        smodelIndexes += childModelCounts[3];
        remainingModelCount -= childModelCounts[3];

        auto childCount = 0u;
        for (const auto childModelCount : childModelCounts)
            childCount += childModelCount != 0u ? 1u : 0u;

        if (childCount == 0u)
            return true;

        if (tree.surfaceCount > 0u)
            childCount++;
        if (remainingModelCount > 0u)
            childCount++;

        auto* children = AllocZeroed<GfxAabbTree>(memory, childCount);
        if (!SetAabbTreeChildrenOffset(tree, children, error))
            return false;

        tree.childCount = 0u;
        if (tree.surfaceCount > 0u)
        {
            auto& childTree = children[tree.childCount++];
            std::memcpy(childTree.mins, tree.mins, sizeof(childTree.mins));
            std::memcpy(childTree.maxs, tree.maxs, sizeof(childTree.maxs));
            childTree.startSurfIndex = tree.startSurfIndex;
            childTree.surfaceCount = tree.surfaceCount;
            childTree.startSurfIndexNoDecal = tree.startSurfIndexNoDecal;
            childTree.surfaceCountNoDecal = tree.surfaceCountNoDecal;
        }

        smodelIndexes = tree.smodelIndexes;
        remainingModelCount = tree.smodelIndexCount;
        for (const auto childModelCount : childModelCounts)
        {
            if (!AddSortedStaticModelChild(tree, smodelIndexes, remainingModelCount, childModelCount, error))
                return false;

            if (childModelCount > 0u && !SortGfxAabbTree(world, children[tree.childCount - 1u], memory, error))
                return false;
        }

        if (remainingModelCount > 0u)
        {
            if (!AddSortedStaticModelChild(tree, smodelIndexes, remainingModelCount, remainingModelCount, error))
                return false;

            if (!SortGfxAabbTree(world, children[tree.childCount - 1u], memory, error))
                return false;
        }

        return true;
    }

    [[nodiscard]] GfxAabbTree* MoveAabbTree_r(GfxAabbTree& tree, GfxAabbTree& newTree, GfxAabbTree* nextChild)
    {
        newTree = tree;
        if (tree.childCount == 0u)
        {
            newTree.childrenOffset = 0;
            return nextChild;
        }

        auto* children = AabbTreeChildren(tree);
        newTree.childrenOffset = static_cast<int>(reinterpret_cast<char*>(nextChild) - reinterpret_cast<char*>(&newTree));
        auto* nextFreeTree = nextChild + tree.childCount;
        for (auto childIndex = 0u; childIndex < tree.childCount; childIndex++)
            nextFreeTree = MoveAabbTree_r(children[childIndex], nextChild[childIndex], nextFreeTree);

        return nextFreeTree;
    }

    [[nodiscard]] bool FixupGfxAabbTrees(GfxCell& cell, MemoryManager& memory, std::string& error)
    {
        if (!cell.aabbTree)
            return true;

        const auto treeCount = AabbTreeSubtreeCount(*cell.aabbTree);
        if (!FitsInt(treeCount))
        {
            error = "too many AABB tree nodes after static model sort";
            return false;
        }

        auto* newTree = AllocZeroed<GfxAabbTree>(memory, treeCount);
        const auto* nextTree = MoveAabbTree_r(*cell.aabbTree, *newTree, newTree + 1);
        if (nextTree != newTree + treeCount)
        {
            error = "AABB tree fixup produced an unexpected node count";
            return false;
        }

        cell.aabbTree = newTree;
        cell.aabbTreeCount = static_cast<int>(treeCount);
        return true;
    }

    [[nodiscard]] bool PopulateWorldStaticModelAabbTrees(GfxWorld& world, MemoryManager& memory, std::string& error)
    {
        if (world.dpvs.smodelCount == 0u || !world.dpvs.smodelInsts || !world.cells || world.dpvsPlanes.cellCount <= 0)
            return true;

        if (world.dpvs.smodelCount > std::numeric_limits<uint16_t>::max())
        {
            error = "too many static models for AABB tree indexes";
            return false;
        }

        StaticModelIndexLists staticModelIndexesByTree;
        for (auto smodelIndex = 0u; smodelIndex < world.dpvs.smodelCount; smodelIndex++)
        {
            const auto packedIndex = static_cast<uint16_t>(smodelIndex);
            const auto& smodelInst = world.dpvs.smodelInsts[smodelIndex];

            if (world.dpvsPlanes.nodes)
            {
                if (!FilterStaticModelIntoCells_r(world, staticModelIndexesByTree, packedIndex, world.dpvsPlanes.nodes, smodelInst.mins, smodelInst.maxs, memory, error))
                    return false;
            }
            else if (!AddStaticModelToCell(world, staticModelIndexesByTree, packedIndex, 0, memory, error))
            {
                return false;
            }
        }

        if (!CommitStaticModelAabbTreeIndexes(staticModelIndexesByTree, memory, error))
            return false;

        // After static models are assigned, linker_pc recursively sorts each
        // tree, splits heavy static-model leaves into smaller buckets, then
        // flattens every cell tree into a contiguous array. The raw
        // SIMPLE_AABBTREES lump is derived from these fixed-up runtime trees.
        for (auto cellIndex = 0; cellIndex < world.dpvsPlanes.cellCount; cellIndex++)
        {
            auto& cell = world.cells[cellIndex];
            if (cell.aabbTree && !SortGfxAabbTree(world, *cell.aabbTree, memory, error))
                return false;
        }

        for (auto cellIndex = 0; cellIndex < world.dpvsPlanes.cellCount; cellIndex++)
        {
            if (!FixupGfxAabbTrees(world.cells[cellIndex], memory, error))
                return false;
        }

        return true;
    }

    [[nodiscard]] bool PopulateWorldDynamicEntities(GfxWorld& world, const clipMap_t* clipMap, MemoryManager& memory, std::string& error)
    {
        if (!clipMap)
            return true;

        if (world.dpvsPlanes.cellCount < 0)
        {
            error = "negative world cell count";
            return false;
        }

        const auto nonSunLightCount = NonSunPrimaryLightCount(world);
        if (nonSunLightCount > 0uz)
        {
            // linker_pc emits this runtime shadow visibility buffer zeroed. It
            // is not sourced from a named d3dbsp lump, but its serialized size
            // is derived from the number of non-sun primary lights.
            world.primaryLightEntityShadowVis = AllocZeroed<unsigned int>(memory, nonSunLightCount * 4096uz);
        }

        for (auto dynType = 0uz; dynType < 2uz; dynType++)
        {
            const auto count = static_cast<unsigned>(clipMap->dynEntCount[dynType]);
            world.dpvsDyn.dynEntClientCount[dynType] = count;
            world.dpvsDyn.dynEntClientWordCount[dynType] = (count + 31u) / 32u;

            if (count == 0u)
                continue;

            // These buffers are runtime visibility state. linker_pc emits them
            // zeroed while sizing them from the clipmap dynamic entity counts.
            if (dynType == 0uz)
                world.sceneDynModel = AllocZeroed<GfxSceneDynModel4>(memory, count);
            else
                world.sceneDynBrush = AllocZeroed<GfxSceneDynBrush>(memory, count);

            const auto wordCount = static_cast<size_t>(world.dpvsDyn.dynEntClientWordCount[dynType]);
            const auto cellCount = static_cast<size_t>(world.dpvsPlanes.cellCount);
            if (cellCount > 0uz)
                world.dpvsDyn.dynEntCellBits[dynType] = AllocZeroed<unsigned int>(memory, wordCount * cellCount);

            for (auto viewIndex = 0uz; viewIndex < 3uz; viewIndex++)
                world.dpvsDyn.dynEntVisData[dynType][viewIndex] = AllocZeroed<raw_byte16>(memory, 32uz * wordCount);

            if (nonSunLightCount > 0uz)
                world.primaryLightDynEntShadowVis[dynType] = AllocZeroed<unsigned int>(memory, static_cast<size_t>(count) * nonSunLightCount);
        }

        if (world.dpvsDyn.dynEntClientCount[0] > 0u)
            world.nonSunPrimaryLightForModelDynEnt = AllocZeroed<char>(memory, world.dpvsDyn.dynEntClientCount[0]);

        return true;
    }

    void PopulateWorldRuntimeData(GfxWorld& world, MemoryManager& memory)
    {
        const auto cellCount = static_cast<size_t>(std::max(world.dpvsPlanes.cellCount, 0));
        const auto cellWordCount = (cellCount + 31uz) >> 5uz;

        // R_LoadWorldRuntime derives these runtime DPVS buffers after raw BSP
        // loading. They are serialized into fastfiles by linker_pc, but they do
        // not have dedicated d3dbsp lumps.
        world.cellBitsCount = static_cast<int>(16uz * ((cellCount + 127uz) >> 7uz));
        if (cellCount > 0uz)
        {
            if (!world.cellCasterBits)
                world.cellCasterBits = AllocZeroed<unsigned int>(memory, cellCount * cellWordCount);
            if (!world.dpvsPlanes.sceneEntCellBits)
                world.dpvsPlanes.sceneEntCellBits = AllocZeroed<unsigned int>(memory, cellCount * 0x100uz);
        }

        if (world.modelCount > 0 && world.models)
        {
            world.dpvs.staticSurfaceCount = world.models[0].surfaceCount;
            world.dpvs.staticSurfaceCountNoDecal = world.models[0].surfaceCountNoDecal;
        }

        if (world.dpvs.staticSurfaceCount > 0u)
        {
            world.dpvs.litSurfsBegin = 0u;
            if (world.dpvs.litSurfsEnd == 0u || world.dpvs.litSurfsEnd > world.dpvs.staticSurfaceCount)
                world.dpvs.litSurfsEnd = world.dpvs.staticSurfaceCount;
            if (world.dpvs.decalSurfsBegin == 0u && world.dpvs.decalSurfsEnd == 0u)
            {
                world.dpvs.decalSurfsBegin = world.dpvs.staticSurfaceCount;
                world.dpvs.decalSurfsEnd = world.dpvs.staticSurfaceCount;
            }
            if (world.dpvs.emissiveSurfsBegin == 0u && world.dpvs.emissiveSurfsEnd == 0u)
            {
                world.dpvs.emissiveSurfsBegin = world.dpvs.staticSurfaceCount;
                world.dpvs.emissiveSurfsEnd = world.dpvs.staticSurfaceCount;
            }
        }

        world.dpvs.smodelVisDataCount = 4u * ((world.dpvs.smodelCount + 127u) >> 7u);
        world.dpvs.surfaceVisDataCount = 4u * ((world.dpvs.staticSurfaceCount + 127u) >> 7u);

        for (auto viewIndex = 0uz; viewIndex < 3uz; viewIndex++)
        {
            if (world.dpvs.smodelCount > 0u && !world.dpvs.smodelVisData[viewIndex])
                world.dpvs.smodelVisData[viewIndex] = AllocZeroed<char>(memory, world.dpvs.smodelCount);
            if (world.dpvs.staticSurfaceCount > 0u && !world.dpvs.surfaceVisData[viewIndex])
                world.dpvs.surfaceVisData[viewIndex] = AllocZeroed<char>(memory, world.dpvs.staticSurfaceCount);
        }

        if (world.dpvs.smodelCount > 0u && !world.dpvs.lodData)
            world.dpvs.lodData = AllocZeroed<raw_uint128>(memory, 2uz * world.dpvs.smodelVisDataCount);

        if (world.dpvs.staticSurfaceCount > 0u)
        {
            if (!world.dpvs.surfaceMaterials)
                world.dpvs.surfaceMaterials = AllocZeroed<GfxDrawSurf>(memory, world.dpvs.staticSurfaceCount);
            if (!world.dpvs.surfaceCastsSunShadow)
                world.dpvs.surfaceCastsSunShadow = AllocZeroed<raw_uint128>(memory, world.dpvs.surfaceVisDataCount);
        }

        const auto sortedSurfIndexCount = static_cast<size_t>(world.dpvs.staticSurfaceCount + world.dpvs.staticSurfaceCountNoDecal);
        if (sortedSurfIndexCount > 0uz && !world.dpvs.sortedSurfIndex)
        {
            world.dpvs.sortedSurfIndex = AllocZeroed<uint16_t>(memory, sortedSurfIndexCount);
            for (auto i = 0uz; i < sortedSurfIndexCount; i++)
            {
                const auto sourceIndex = world.surfaceCount > 0 ? i % static_cast<size_t>(world.surfaceCount) : 0uz;
                world.dpvs.sortedSurfIndex[i] = static_cast<uint16_t>(std::min(sourceIndex, static_cast<size_t>(UINT16_MAX)));
            }
        }
    }

    struct DpvsNodeLoad
    {
        int planeIndex = -1;
        int children[2]{};
        int cellIndex = -2;
    };

    [[nodiscard]] bool SetDpvsNodeCells_r(std::vector<DpvsNodeLoad>& nodes, std::vector<uint8_t>& visitState, const size_t nodeIndex, const size_t rawNodeCount)
    {
        if (nodeIndex >= nodes.size())
            return false;

        if (nodeIndex >= rawNodeCount)
            return true;

        if (visitState[nodeIndex] == 1u)
            return false;

        if (visitState[nodeIndex] == 2u)
            return true;

        visitState[nodeIndex] = 1u;
        auto& node = nodes[nodeIndex];
        if (!SetDpvsNodeCells_r(nodes, visitState, static_cast<size_t>(node.children[0]), rawNodeCount)
            || !SetDpvsNodeCells_r(nodes, visitState, static_cast<size_t>(node.children[1]), rawNodeCount))
        {
            return false;
        }

        node.cellIndex = -2;
        if (nodes[static_cast<size_t>(node.children[0])].cellIndex == nodes[static_cast<size_t>(node.children[1])].cellIndex)
            node.cellIndex = nodes[static_cast<size_t>(node.children[0])].cellIndex;

        visitState[nodeIndex] = 2u;
        return true;
    }

    [[nodiscard]] bool CountDpvsNodeStream_r(const std::vector<DpvsNodeLoad>& nodes, const size_t nodeIndex, size_t& count)
    {
        if (nodeIndex >= nodes.size())
            return false;

        const auto& node = nodes[nodeIndex];
        if (node.cellIndex != -2)
        {
            count++;
            return true;
        }

        count += 2uz;
        return CountDpvsNodeStream_r(nodes, static_cast<size_t>(node.children[0]), count)
               && CountDpvsNodeStream_r(nodes, static_cast<size_t>(node.children[1]), count);
    }

    [[nodiscard]] bool WriteDpvsNodeStream_r(
        const std::vector<DpvsNodeLoad>& nodes,
        const size_t nodeIndex,
        const int cellCount,
        uint16_t*& out)
    {
        const auto& node = nodes[nodeIndex];
        if (node.cellIndex != -2)
        {
            const auto cellValue = node.cellIndex + 1;
            if (cellValue < 0 || cellValue > static_cast<int>(std::numeric_limits<uint16_t>::max()))
                return false;

            *out++ = static_cast<uint16_t>(cellValue);
            return true;
        }

        const auto internalValue = cellCount + node.planeIndex + 1;
        if (internalValue < 0 || internalValue > static_cast<int>(std::numeric_limits<uint16_t>::max()))
            return false;

        auto* current = out;
        *out++ = static_cast<uint16_t>(internalValue);
        auto* rightChildOffset = out++;

        if (!WriteDpvsNodeStream_r(nodes, static_cast<size_t>(node.children[0]), cellCount, out))
            return false;

        const auto offset = static_cast<size_t>(out - current);
        if (!FitsUint16(offset))
            return false;

        *rightChildOffset = static_cast<uint16_t>(offset);
        return WriteDpvsNodeStream_r(nodes, static_cast<size_t>(node.children[1]), cellCount, out);
    }

    [[nodiscard]] bool PopulateWorldDpvsNodes(GfxWorld& world, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        const auto* rawNodes = bsp.GetLump(LUMP_NODES);
        const auto* rawLeafs = bsp.GetLump(LUMP_LEAFS);
        if (!rawNodes || rawNodes->data.empty() || !rawLeafs || rawLeafs->data.empty())
        {
            if (world.dpvsPlanes.cellCount <= 0)
                return true;

            // A valid runtime world still needs a root node for bounds-to-cell
            // filtering. A single leaf routes all dynamic entities to cell 0.
            world.nodeCount = 1;
            world.dpvsPlanes.nodes = AllocZeroed<uint16_t>(memory, 1uz);
            world.dpvsPlanes.nodes[0] = 1u;
            return true;
        }

        if (rawNodes->data.size() % RAW_CLIP_NODE_SIZE != 0uz || rawLeafs->data.size() % RAW_LEAF_SIZE != 0uz)
        {
            error = "world node/leaf lump has funny size";
            return false;
        }

        const auto rawNodeCount = RecordCount(*rawNodes, RAW_CLIP_NODE_SIZE);
        const auto rawLeafCount = RecordCount(*rawLeafs, RAW_LEAF_SIZE);
        if (rawNodeCount == 0uz || rawLeafCount == 0uz)
        {
            error = "world node tree is empty";
            return false;
        }

        if (!FitsInt(rawNodeCount) || rawLeafCount > std::numeric_limits<size_t>::max() - rawNodeCount || !FitsInt(rawNodeCount + rawLeafCount))
        {
            error = "world node tree is too large";
            return false;
        }

        std::vector<DpvsNodeLoad> nodes(rawNodeCount + rawLeafCount);
        for (auto nodeIndex = 0uz; nodeIndex < rawNodeCount; nodeIndex++)
        {
            const auto* record = rawNodes->data.data() + nodeIndex * RAW_CLIP_NODE_SIZE;
            auto& node = nodes[nodeIndex];
            node.planeIndex = ReadI32(record);
            node.cellIndex = -2;

            if (node.planeIndex < 0 || node.planeIndex >= world.planeCount)
            {
                error = "world node references invalid plane";
                return false;
            }

            for (auto childIndex = 0uz; childIndex < 2uz; childIndex++)
            {
                const auto rawChild = ReadI32(record, 4uz + childIndex * sizeof(int32_t));
                const auto convertedChild = rawChild < 0 ? static_cast<int64_t>(rawNodeCount) - 1ll - rawChild : static_cast<int64_t>(rawChild);
                if (convertedChild < 0 || static_cast<size_t>(convertedChild) >= nodes.size())
                {
                    error = "world node references invalid child";
                    return false;
                }

                node.children[childIndex] = static_cast<int>(convertedChild);
            }
        }

        for (auto leafIndex = 0uz; leafIndex < rawLeafCount; leafIndex++)
        {
            const auto cellIndex = ReadI32(rawLeafs->data.data() + leafIndex * RAW_LEAF_SIZE, RAW_LEAF_CELL_INDEX_OFFSET);
            if (cellIndex < -1 || cellIndex >= world.dpvsPlanes.cellCount)
            {
                error = "world leaf references invalid cell";
                return false;
            }

            nodes[rawNodeCount + leafIndex].cellIndex = cellIndex;
        }

        std::vector<uint8_t> visitState(rawNodeCount);
        if (!SetDpvsNodeCells_r(nodes, visitState, 0uz, rawNodeCount))
        {
            error = "world node tree is cyclic or invalid";
            return false;
        }

        auto streamCount = 0uz;
        if (!CountDpvsNodeStream_r(nodes, 0uz, streamCount) || !FitsInt(streamCount))
        {
            error = "world node stream is too large";
            return false;
        }

        world.nodeCount = static_cast<int>(streamCount);
        world.dpvsPlanes.nodes = AllocZeroed<uint16_t>(memory, streamCount);

        auto* out = world.dpvsPlanes.nodes;
        if (!WriteDpvsNodeStream_r(nodes, 0uz, world.dpvsPlanes.cellCount, out) || static_cast<size_t>(out - world.dpvsPlanes.nodes) != streamCount)
        {
            error = "world node stream could not be packed";
            return false;
        }

        return true;
    }

    [[nodiscard]] bool PopulateWorldDpvsPlanes(
        GfxWorld& world, const clipMap_t* clipMap, const IW3::d3dbsp::File& bsp, MemoryManager& memory, std::string& error)
    {
        if (!clipMap)
            return true;

        if (clipMap->planeCount > 0 && clipMap->planes)
        {
            world.planeCount = clipMap->planeCount;
            world.dpvsPlanes.planes = AllocZeroed<cplane_s>(memory, static_cast<size_t>(clipMap->planeCount));
            std::memcpy(world.dpvsPlanes.planes, clipMap->planes, static_cast<size_t>(clipMap->planeCount) * sizeof(cplane_s));
        }

        return PopulateWorldDpvsNodes(world, bsp, memory, error);
    }

    class ClipMapPvsLoader final : public AssetCreator<AssetClipMapPvs>
    {
    public:
        ClipMapPvsLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            auto* clipMap = AllocZeroed<clipMap_t>(m_memory);
            clipMap->name = m_memory.Dup(assetName.c_str());
            clipMap->isInUse = 1;

            std::string error;
            if (!PopulateClipMapMaterials(*clipMap, *bsp, m_memory, error) || !PopulateClipMapPlanes(*clipMap, *bsp, m_memory, error)
                || !PopulateClipMapBrushes(*clipMap, *bsp, m_memory, error) || !PopulateClipMapNodes(*clipMap, *bsp, m_memory, error)
                || !PopulateClipMapLeafBrushes(*clipMap, *bsp, m_memory, error) || !PopulateClipMapCollision(*clipMap, *bsp, m_memory, error)
                || !PopulateClipMapLeafs(*clipMap, *bsp, m_memory, error) || !PopulateClipMapModels(*clipMap, *bsp, m_memory, error)
                || !PopulateClipMapVisibility(*clipMap, *bsp, m_memory, error) || !PopulateClipMapLeafSurfaces(*clipMap, *bsp, m_memory, error))
            {
                con::error("Could not create clipmap \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            std::vector<EntityBlock> entityBlocks;
            const auto* entityLump = bsp->GetLump(LUMP_ENTITIES);
            if (entityLump)
            {
                std::string parseError;
                if (!ParseEntityBlocks(entityLump->data, entityBlocks, parseError))
                {
                    con::error("Could not create clipmap \"{}\" from {}: {}", assetName, bsp->m_file_name, parseError);
                    return AssetCreationResult::Failure();
                }
            }

            const auto staticModelBlocks = StaticModelEntityBlocks(entityBlocks);
            const auto staticModelDependencies = LoadStaticModelDependencies(staticModelBlocks, context);
            const auto dynModelBlocks = DynModelEntityBlocks(entityBlocks);
            const auto dynModelDependencies = LoadDynModelDependencies(dynModelBlocks, context);
            PopulateStaticModels(*clipMap, staticModelBlocks, staticModelDependencies, m_memory);
            PopulateDynModelEntities(*clipMap, dynModelBlocks, dynModelDependencies, context, m_memory);

            auto* mapEntsDependency = context.LoadDependency<AssetMapEnts>(assetName);
            if (mapEntsDependency)
                clipMap->mapEnts = mapEntsDependency->Asset();

            AssetRegistration<AssetClipMapPvs> registration(assetName, clipMap);
            if (mapEntsDependency)
                registration.AddDependency(mapEntsDependency);
            for (auto* dependency : staticModelDependencies)
            {
                if (dependency)
                    registration.AddDependency(dependency);
            }
            for (const auto& dependency : dynModelDependencies)
            {
                if (dependency.model)
                    registration.AddDependency(dependency.model);
                if (dependency.physPreset)
                    registration.AddDependency(dependency.physPreset);
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };

    class MapEntsLoader final : public AssetCreator<AssetMapEnts>
    {
    public:
        MapEntsLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            const auto* entities = bsp->GetLump(LUMP_ENTITIES);
            if (!entities)
            {
                con::error("Could not create MapEnts \"{}\" from {}: missing entity lump", assetName, bsp->m_file_name);
                return AssetCreationResult::Failure();
            }

            std::vector<EntityBlock> entityBlocks;
            std::string parseError;
            if (!ParseEntityBlocks(entities->data, entityBlocks, parseError))
            {
                con::error("Could not create MapEnts \"{}\" from {}: {}", assetName, bsp->m_file_name, parseError);
                return AssetCreationResult::Failure();
            }

            const auto compiledEntityString = CompileMapEntsEntityString(entityBlocks);
            const auto entityCharCount = compiledEntityString.size();
            if (!FitsInt(entityCharCount))
            {
                con::error("Could not create MapEnts \"{}\" from {}: entity lump is too large", assetName, bsp->m_file_name);
                return AssetCreationResult::Failure();
            }

            auto* entityString = m_memory.Alloc<char>(std::max(1uz, entityCharCount));
            std::memcpy(entityString, compiledEntityString.data(), compiledEntityString.size());

            auto* mapEnts = m_memory.Alloc<MapEnts>();
            mapEnts->name = m_memory.Dup(assetName.c_str());
            mapEnts->entityString = entityString;
            mapEnts->numEntityChars = static_cast<int>(entityCharCount);

            return AssetCreationResult::Success(context.AddAsset<AssetMapEnts>(assetName, mapEnts));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };

    [[nodiscard]] bool DecodePathVisRle(
        const std::vector<std::byte>& data, size_t& offset, const size_t expectedSize, MemoryManager& memory, char*& pathVis, std::string& error)
    {
        pathVis = expectedSize > 0uz ? AllocZeroed<char>(memory, expectedSize) : nullptr;
        auto outOffset = 0uz;

        while (outOffset < expectedSize)
        {
            if (offset >= data.size())
            {
                error = "path visibility RLE ended early";
                return false;
            }

            const auto marker = std::to_integer<unsigned char>(data[offset++]);
            if ((marker & 0x80u) == 0u)
            {
                const auto zeroCount = static_cast<size_t>(marker);
                if (zeroCount > expectedSize - outOffset || offset >= data.size())
                {
                    error = "path visibility zero run exceeds expected size";
                    return false;
                }

                outOffset += zeroCount;
                pathVis[outOffset++] = static_cast<char>(std::to_integer<unsigned char>(data[offset++]));
            }
            else
            {
                const auto literalCount = static_cast<size_t>(static_cast<unsigned char>(~marker));
                if (literalCount > expectedSize - outOffset || literalCount > data.size() - offset)
                {
                    error = "path visibility literal run exceeds expected size";
                    return false;
                }

                std::memcpy(pathVis + outOffset, data.data() + offset, literalCount);
                outOffset += literalCount;
                offset += literalCount;
            }
        }

        return true;
    }

    class GameWorldSpLoader final : public AssetCreator<AssetGameWorldSp>
    {
    public:
        GameWorldSpLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            const auto* pathConnections = bsp->GetLump(LUMP_PATHCONNECTIONS);
            if (!pathConnections)
                return AssetCreationResult::NoAction();

            const auto& data = pathConnections->data;
            if (data.size() < sizeof(uint32_t) + sizeof(uint16_t))
            {
                con::error("Could not create GameWorldSp \"{}\" from {}: pathconnections lump is truncated", assetName, bsp->m_file_name);
                return AssetCreationResult::Failure();
            }

            auto offset = 0uz;
            const auto version = ReadU32(data.data() + offset);
            offset += sizeof(uint32_t);
            if (version != PATHCONNECTIONS_VERSION)
            {
                con::error("Could not create GameWorldSp \"{}\" from {}: unsupported pathconnections version {}", assetName, bsp->m_file_name, version);
                return AssetCreationResult::Failure();
            }

            const auto nodeCount = ReadU16(data.data() + offset);
            offset += sizeof(uint16_t);

            auto* gameWorld = AllocZeroed<GameWorldSp>(m_memory);
            gameWorld->name = m_memory.Dup(assetName.c_str());
            gameWorld->path.nodeCount = nodeCount;
            gameWorld->path.nodes = nodeCount > 0u ? AllocZeroed<pathnode_t>(m_memory, nodeCount) : nullptr;
            gameWorld->path.basenodes = nodeCount > 0u ? AllocZeroed<pathbasenode_t>(m_memory, nodeCount) : nullptr;

            for (auto nodeIndex = 0uz; nodeIndex < nodeCount; nodeIndex++)
            {
                if (offset + sizeof(uint16_t) > data.size())
                {
                    con::error("Could not create GameWorldSp \"{}\" from {}: path link counts are truncated", assetName, bsp->m_file_name);
                    return AssetCreationResult::Failure();
                }

                auto& node = gameWorld->path.nodes[nodeIndex];
                const auto linkCount = ReadU16(data.data() + offset);
                offset += sizeof(uint16_t);
                node.constant.totalLinkCount = linkCount;
                node.constant.Links = linkCount > 0u ? AllocZeroed<pathlink_s>(m_memory, linkCount) : nullptr;

                for (auto linkIndex = 0uz; linkIndex < linkCount; linkIndex++)
                {
                    if (offset + sizeof(uint16_t) + sizeof(float) > data.size())
                    {
                        con::error("Could not create GameWorldSp \"{}\" from {}: path links are truncated", assetName, bsp->m_file_name);
                        return AssetCreationResult::Failure();
                    }

                    auto& link = node.constant.Links[linkIndex];
                    link.nodeNum = ReadU16(data.data() + offset);
                    offset += sizeof(uint16_t);
                    link.fDist = ReadFloat(data.data() + offset);
                    offset += sizeof(float);
                }
            }

            const auto visBytes = (static_cast<size_t>(nodeCount) * (static_cast<size_t>(nodeCount) - 1uz) + 7uz) >> 3uz;
            gameWorld->path.visBytes = static_cast<int>(visBytes);

            std::string error;
            if (!DecodePathVisRle(data, offset, visBytes, m_memory, gameWorld->path.pathVis, error))
            {
                con::error("Could not create GameWorldSp \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset<AssetGameWorldSp>(assetName, gameWorld));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };

    class GameWorldMpLoader final : public AssetCreator<AssetGameWorldMp>
    {
    public:
        GameWorldMpLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            auto* gameWorld = AllocZeroed<GameWorldMp>(m_memory);
            gameWorld->name = m_memory.Dup(assetName.c_str());
            return AssetCreationResult::Success(context.AddAsset<AssetGameWorldMp>(assetName, gameWorld));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };

    [[nodiscard]] bool PopulateWorldSkySurfaces(
        GfxWorld& world, AssetCreationContext& context, AssetRegistration<AssetGfxWorld>& registration, MemoryManager& memory, std::string& error)
    {
        if (!world.dpvs.surfaces || world.surfaceCount <= 0)
            return true;

        std::vector<int> skySurfaces;
        const Material* skyMaterial = nullptr;
        for (auto surfaceIndex = 0; surfaceIndex < world.surfaceCount; surfaceIndex++)
        {
            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            if (!surface.material || (surface.material->info.gameFlags & 8u) == 0u)
                continue;

            // linker_pc identifies sky surfaces from the material game flag,
            // not from the raw sky lightmap index. It also supports exactly
            // one sky material and uses that material's colorMap cubemap as
            // GfxWorld::skyImage.
            if (skyMaterial && skyMaterial != surface.material)
            {
                error = std::format("map has at least two different skies: {} and {}", surface.material->info.name, skyMaterial->info.name);
                return false;
            }

            skyMaterial = surface.material;
            skySurfaces.emplace_back(surfaceIndex);
        }

        if (skySurfaces.empty())
            return true;

        constexpr auto colorMapHash = Common::R_HashString("colorMap");
        for (auto textureIndex = 0uz; textureIndex < skyMaterial->textureCount; textureIndex++)
        {
            const auto& texture = skyMaterial->textureTable[textureIndex];
            if (texture.nameHash != colorMapHash)
                continue;

            const auto* image = texture.u.image;
            if (!image || texture.semantic == TS_WATER_MAP || image->mapType != MAPTYPE_CUBE)
            {
                error = std::format("colorMap for sky material \"{}\" is not a cubemap", skyMaterial->info.name);
                return false;
            }

            auto* imageDependency = context.LoadDependency<AssetImage>(image->name);
            if (!imageDependency)
            {
                error = std::format("missing sky image \"{}\"", image->name);
                return false;
            }

            registration.AddDependency(imageDependency);
            world.skyImage = imageDependency->Asset();
            world.skySamplerState = static_cast<char>(SamplerStateByte(texture.samplerState));
            break;
        }

        if (!world.skyImage)
        {
            error = std::format("sky material \"{}\" has no colorMap", skyMaterial->info.name);
            return false;
        }

        world.skySurfCount = static_cast<int>(skySurfaces.size());
        world.skyStartSurfs = AllocZeroed<int>(memory, skySurfaces.size());
        std::memcpy(world.skyStartSurfs, skySurfaces.data(), skySurfaces.size() * sizeof(int));
        return true;
    }

    void SetOutdoorLookupIdentity(GfxWorld& world)
    {
        for (auto row = 0uz; row < 4uz; row++)
        {
            for (auto column = 0uz; column < 4uz; column++)
                world.outdoorLookupMatrix[row][column] = row == column ? 1.0f : 0.0f;
        }
    }

    [[nodiscard]] bool PopulateWorldOutdoorData(
        GfxWorld& world,
        const IW3::d3dbsp::File& bsp,
        AssetCreationContext& context,
        AssetRegistration<AssetGfxWorld>& registration,
        MemoryManager& memory,
        std::string& error)
    {
        const auto* materials = bsp.GetLump(LUMP_MATERIALS);
        const auto* surfaces = SelectWorldLumpForTrisType(bsp, LUMP_LAYERED_TRI_SOUPS, LUMP_SIMPLE_TRI_SOUPS);
        if (!materials || !surfaces || world.modelCount <= 0 || !world.models || !world.dpvs.surfaces)
            return true;

        if (materials->data.size() % RAW_MATERIAL_SIZE != 0uz || surfaces->data.size() % RAW_WORLD_SURFACE_SIZE != 0uz)
        {
            error = "could not calculate outdoor bounds from funny-sized material or surface lump";
            return false;
        }

        const auto rawMaterialCount = RecordCount(*materials, RAW_MATERIAL_SIZE);
        const auto rawSurfaceCount = RecordCount(*surfaces, RAW_WORLD_SURFACE_SIZE);
        const auto& rootModel = world.models[0];
        const auto rootStartSurface = static_cast<size_t>(rootModel.startSurfIndex);
        const auto rootSurfaceCount = static_cast<size_t>(rootModel.surfaceCount);
        if (rootStartSurface + rootSurfaceCount > rawSurfaceCount || rootStartSurface + rootSurfaceCount > static_cast<size_t>(world.surfaceCount))
        {
            error = "root model surface range is outside the world surface lump";
            return false;
        }

        float outdoorMins[3]{131072.0f, 131072.0f, 131072.0f};
        float outdoorMaxs[3]{-131072.0f, -131072.0f, -131072.0f};
        for (auto surfaceIndex = rootStartSurface; surfaceIndex < rootStartSurface + rootSurfaceCount; surfaceIndex++)
        {
            const auto* rawSurface = surfaces->data.data() + surfaceIndex * RAW_WORLD_SURFACE_SIZE;
            const auto rawMaterialIndex = static_cast<size_t>(ReadU16(rawSurface));
            if (rawMaterialIndex >= rawMaterialCount)
            {
                error = std::format("world surface {} references invalid material index {}", surfaceIndex, rawMaterialIndex);
                return false;
            }

            const auto& surface = world.dpvs.surfaces[surfaceIndex];
            const auto rawContentFlags = ReadI32(materials->data.data() + rawMaterialIndex * RAW_MATERIAL_SIZE, 68uz);
            // R_CalculateOutdoorBounds includes only non-sky root-model
            // surfaces whose raw BSP material contents carry outdoor-relevant
            // solid/detail bits. The generated matrix is consumed by outdoor
            // shader code through TEXTURE_SRC_CODE_OUTDOOR.
            if (surface.material && (surface.material->info.gameFlags & 8u) == 0u && (rawContentFlags & 0x2001) != 0)
                ExpandBounds(surface.bounds[0], surface.bounds[1], outdoorMins, outdoorMaxs);
        }

        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            if (outdoorMins[axis] == 131072.0f)
            {
                outdoorMins[axis] = 0.0f;
                outdoorMaxs[axis] = 0.0f;
            }

            if (outdoorMaxs[axis] - outdoorMins[axis] < 1.0f)
            {
                outdoorMins[axis] -= 0.5f;
                outdoorMaxs[axis] += 0.5f;
            }
        }

        SetOutdoorLookupIdentity(world);
        for (auto axis = 0uz; axis < 3uz; axis++)
        {
            const auto scale = LinkerFloat(1.0 / static_cast<double>(outdoorMaxs[axis] - outdoorMins[axis]));
            world.outdoorLookupMatrix[axis][axis] = scale;
            world.outdoorLookupMatrix[3][axis] = LinkerFloat(-static_cast<double>(outdoorMins[axis]) * scale);
        }

        constexpr auto OUTDOOR_IMAGE_SIZE = 512u;
        constexpr auto OUTDOOR_IMAGE_DATA_SIZE = static_cast<size_t>(OUTDOOR_IMAGE_SIZE) * OUTDOOR_IMAGE_SIZE;
        std::vector<std::byte> pixels(OUTDOOR_IMAGE_DATA_SIZE);

        // linker_pc generates this image by tracing height through the loaded
        // clipmap. Keep the serialized image shape/name identical for now; the
        // exact texel-generation pass can be added once the collision trace
        // path is complete.
        auto* image = CreateGeneratedImage(memory,
                                           OutdoorImageName(),
                                           MAPTYPE_2D,
                                           TS_FUNCTION,
                                           IMG_CATEGORY_AUTO_GENERATED,
                                           OUTDOOR_IMAGE_SIZE,
                                           OUTDOOR_IMAGE_SIZE,
                                           1u,
                                           oat::D3DFMT_L8,
                                           static_cast<char>(image::iwi6::IMG_FLAG_NOMIPMAPS),
                                           pixels.data(),
                                           pixels.size());
        auto* imageInfo = AddGeneratedImage(context, registration, OutdoorImageName(), image);
        if (!imageInfo)
        {
            error = "could not register generated outdoor image";
            return false;
        }

        world.outdoorImage = imageInfo->Asset();
        return true;
    }

    class GfxWorldLoader final : public AssetCreator<AssetGfxWorld>
    {
    public:
        GfxWorldLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            auto* clipMapDependency = context.LoadDependency<AssetClipMapPvs>(assetName);
            auto* mapEntsDependency = context.LoadDependency<AssetMapEnts>(assetName);
            auto* comWorldDependency = context.LoadDependency<AssetComWorld>(assetName);
            if (!clipMapDependency || !mapEntsDependency || !comWorldDependency)
            {
                con::error("Could not create GfxWorld \"{}\" from {}: required sibling map asset is missing", assetName, bsp->m_file_name);
                return AssetCreationResult::Failure();
            }

            std::vector<EntityBlock> entityBlocks;
            const auto* entityLump = bsp->GetLump(LUMP_ENTITIES);
            if (entityLump)
            {
                std::string parseError;
                if (!ParseEntityBlocks(entityLump->data, entityBlocks, parseError))
                {
                    con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, parseError);
                    return AssetCreationResult::Failure();
                }
            }

            std::string error;
            const auto materialDependencies = LoadWorldMaterials(*bsp, context, m_memory, error);
            if (!error.empty())
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            const auto staticModelBlocks = StaticModelEntityBlocks(entityBlocks);
            const auto staticModelDependencies = LoadStaticModelDependencies(staticModelBlocks, context);

            LightmapAtlasLayout lightmapLayout;
            if (!BuildLightmapAtlasLayout(*bsp, lightmapLayout, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            auto* world = AllocZeroed<GfxWorld>(m_memory);
            world->name = m_memory.Dup(assetName.c_str());
            const auto baseName = BspBaseName(assetName);
            world->baseName = m_memory.Dup(baseName.c_str());
            SetOutdoorLookupIdentity(*world);

            AssetRegistration<AssetGfxWorld> registration(assetName, world);
            registration.AddDependency(clipMapDependency);
            registration.AddDependency(mapEntsDependency);
            registration.AddDependency(comWorldDependency);
            for (auto* dependency : materialDependencies)
            {
                if (dependency)
                    registration.AddDependency(dependency);
            }
            for (auto* dependency : staticModelDependencies)
            {
                if (dependency)
                    registration.AddDependency(dependency);
            }

            auto lightDefDependencies = LoadPrimaryLightDefDependencies(*bsp, context, registration, error);
            if (!error.empty())
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            const auto* clipMap = clipMapDependency->Asset();
            if (!PopulateWorldIndices(*world, *bsp, m_memory, error) || !PopulateWorldVertices(*world, *bsp, m_memory, error)
                || !PopulateWorldSurfaces(*world, *bsp, lightmapLayout, materialDependencies, m_memory, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            PopulateWorldMaterialMemory(*world, m_memory);

            if (!PopulateWorldVertexLayerData(*world, *bsp, m_memory, error) || !PopulateWorldModels(*world, *bsp, m_memory, error)
                || !PopulateWorldCells(*world, *bsp, m_memory, error)
                || !PopulateWorldSurfaceOrganization(*world, m_memory, error)
                || !PopulateWorldDpvsPlanes(*world, clipMap, *bsp, m_memory, error)
                || !PopulateWorldPortals(*world, *bsp, m_memory, error)
                || !PopulateWorldPrimaryLights(*world, *bsp, m_memory, error) || !PopulateWorldShadowGeometry(*world, m_memory)
                || !PopulateWorldLightGrid(*world, *bsp, m_memory, error) || !PopulateWorldLightRegions(*world, *bsp, m_memory, error)
                || !PopulateWorldStaticModels(*world, clipMap, staticModelBlocks, staticModelDependencies, m_memory, error)
                || !PopulateWorldReflectionProbes(*world, *bsp, context, registration, m_memory, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            PopulateWorldStaticModelReflectionProbes(*world);

            if (!PopulateWorldStaticModelAabbTrees(*world, m_memory, error)
                || !PopulateWorldDynamicEntities(*world, clipMap, m_memory, error)
                || !PopulateWorldLightmaps(*world, *bsp, lightmapLayout, lightDefDependencies, m_search_path, context, registration, m_memory, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            PopulateWorldRuntimeData(*world, m_memory);
            if (!PopulateWorldSkySurfaces(*world, context, registration, m_memory, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }
            if (!PopulateWorldOutdoorData(*world, *bsp, context, registration, m_memory, error))
            {
                con::error("Could not create GfxWorld \"{}\" from {}: {}", assetName, bsp->m_file_name, error);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };

    class ComWorldLoader final : public AssetCreator<AssetComWorld>
    {
    public:
        ComWorldLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto* bsp = GetBspForAsset(assetName, m_search_path, context);
            if (!bsp)
                return BspWasInvalid(assetName, m_search_path, context) ? AssetCreationResult::Failure() : AssetCreationResult::NoAction();

            const auto* primaryLightsLump = bsp->GetLump(LUMP_PRIMARY_LIGHTS);
            const auto primaryLightsSize = primaryLightsLump ? primaryLightsLump->data.size() : 0uz;
            if (primaryLightsSize % IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE != 0uz)
            {
                con::error("Could not create ComWorld \"{}\" from {}: primary-light lump has invalid size {}", assetName, bsp->m_file_name, primaryLightsSize);
                return AssetCreationResult::Failure();
            }

            const auto primaryLightCount = primaryLightsSize / IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE;
            if (primaryLightCount > static_cast<size_t>(std::numeric_limits<unsigned int>::max()))
            {
                con::error("Could not create ComWorld \"{}\" from {}: too many primary lights", assetName, bsp->m_file_name);
                return AssetCreationResult::Failure();
            }

            auto* comWorld = m_memory.Alloc<ComWorld>();
            comWorld->name = m_memory.Dup(assetName.c_str());
            comWorld->isInUse = 1;
            comWorld->primaryLightCount = static_cast<unsigned int>(primaryLightCount);
            comWorld->primaryLights = nullptr;

            if (primaryLightCount > 0uz)
            {
                comWorld->primaryLights = m_memory.Alloc<ComPrimaryLight>(primaryLightCount);
                for (auto lightIndex = 0uz; lightIndex < primaryLightCount; lightIndex++)
                {
                    const auto* record = primaryLightsLump->data.data() + lightIndex * IW3::d3dbsp::RAW_PRIMARY_LIGHT_SIZE;
                    ParsePrimaryLightRecord(record, comWorld->primaryLights[lightIndex], m_memory);
                }
            }

            return AssetCreationResult::Success(context.AddAsset<AssetComWorld>(assetName, comWorld));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace map_d3dbsp
{
    std::unique_ptr<AssetCreator<AssetClipMapPvs>> CreateClipMapPvsLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ClipMapPvsLoader>(memory, searchPath);
    }

    std::unique_ptr<AssetCreator<AssetMapEnts>> CreateMapEntsLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<MapEntsLoader>(memory, searchPath);
    }

    std::unique_ptr<AssetCreator<AssetComWorld>> CreateComWorldLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ComWorldLoader>(memory, searchPath);
    }

    std::unique_ptr<AssetCreator<AssetGameWorldSp>> CreateGameWorldSpLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<GameWorldSpLoader>(memory, searchPath);
    }

    std::unique_ptr<AssetCreator<AssetGameWorldMp>> CreateGameWorldMpLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<GameWorldMpLoader>(memory, searchPath);
    }

    std::unique_ptr<AssetCreator<AssetGfxWorld>> CreateGfxWorldLoaderIW3(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<GfxWorldLoader>(memory, searchPath);
    }
} // namespace map_d3dbsp
