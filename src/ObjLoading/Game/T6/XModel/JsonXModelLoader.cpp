#include "JsonXModelLoader.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"
#include "ObjLoading.h"
#include "Utils/QuatInt16.h"
#include "Utils/StringUtils.h"
#include "XModel/Gltf/GltfBinInput.h"
#include "XModel/Gltf/GltfLoader.h"
#include "XModel/Gltf/GltfTextInput.h"
#include "XModel/XModelCommon.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include "XModel/Tangentspace.h"

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <numeric>
#include <vector>

using namespace nlohmann;
using namespace T6;

namespace fs = std::filesystem;

namespace
{
    const char* HITLOC_NAMES[]{
        // clang-format off
        "none",
        "helmet",
        "head",
        "neck",
        "torso_upper",
        "torso_middle",
        "torso_lower",
        "right_arm_upper",
        "left_arm_upper",
        "right_arm_lower",
        "left_arm_lower",
        "right_hand",
        "left_hand",
        "right_leg_upper",
        "left_leg_upper",
        "right_leg_lower",
        "left_leg_lower",
        "right_foot",
        "left_foot",
        "gun",
        "shield",
        // clang-format on
    };
    static_assert(std::extent_v<decltype(HITLOC_NAMES)> == HITLOC_COUNT);

    class PartClassificationState final : public IZoneAssetLoaderState
    {
        // TODO: Use MP part classifications when building an mp fastfile
        static constexpr auto PART_CLASSIFICATION_FILE = "partclassification.csv";

    public:
        PartClassificationState()
            : m_loaded(false)
        {
        }

        bool Load(const IAssetLoadingManager& manager)
        {
            if (m_loaded)
                return true;

            if (ObjLoading::Configuration.Verbose)
                std::cout << "Loading part classification...\n";

            const auto file = manager.GetAssetLoadingContext()->m_raw_search_path->Open(PART_CLASSIFICATION_FILE);
            if (!file.IsOpen())
            {
                std::cerr << std::format("Could not load part classification: Failed to open {}\n", PART_CLASSIFICATION_FILE);
                return false;
            }

            CsvInputStream csvStream(*file.m_stream);
            std::vector<std::string> row;
            auto rowIndex = 0u;
            while (csvStream.NextRow(row))
            {
                if (!LoadRow(rowIndex++, row))
                    return false;
            }

            m_loaded = true;

            return false;
        }

        [[nodiscard]] unsigned GetPartClassificationForBoneName(const std::string& boneName) const
        {
            const auto entry = m_part_classifications.find(boneName);

            return entry != m_part_classifications.end() ? entry->second : HITLOC_NONE;
        }

    private:
        bool LoadRow(const unsigned rowIndex, std::vector<std::string>& row)
        {
            if (row.empty())
                return true;

            if (row.size() != 2)
            {
                std::cerr << "Could not load part classification: Invalid row\n";
                return false;
            }

            utils::MakeStringLowerCase(row[0]);
            utils::MakeStringLowerCase(row[1]);

            const auto foundHitLoc = std::ranges::find(HITLOC_NAMES, row[1]);
            if (foundHitLoc == std::end(HITLOC_NAMES))
            {
                std::cerr << std::format("Invalid hitloc name in row {}: {}\n", rowIndex + 1, row[1]);
                return false;
            }

            const auto hitLocNum = std::distance(std::begin(HITLOC_NAMES), foundHitLoc);

            m_part_classifications.emplace(row[0], hitLocNum);
            return true;
        }

        bool m_loaded;
        std::unordered_map<std::string, unsigned> m_part_classifications;
    };

    class TangentData
    {
    public:
        void CreateTangentData(const XModelCommon& common)
        {
            if (common.m_vertices.empty())
                return;

            const auto vertexCount = common.m_vertices.size();
            m_tangents.resize(vertexCount);
            m_binormals.resize(vertexCount);

            auto triCount = 0u;
            for (const auto& object : common.m_objects)
                triCount += object.m_faces.size();

            std::vector<uint16_t> indices(triCount * 3u);
            auto triOffset = 0u;
            for (const auto& object : common.m_objects)
            {
                for (const auto& face : object.m_faces)
                {
                    indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[0]);
                    indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[1]);
                    indices[triOffset++] = static_cast<uint16_t>(face.vertexIndex[2]);
                }
            }

            const auto& firstVertex = common.m_vertices[0];

            tangent_space::VertexData vertexData{
                firstVertex.coordinates,
                sizeof(XModelVertex),
                firstVertex.normal,
                sizeof(XModelVertex),
                firstVertex.uv,
                sizeof(XModelVertex),
                m_tangents.data(),
                sizeof(float) * 3,
                m_binormals.data(),
                sizeof(float) * 3,
                indices.data(),
            };

            tangent_space::CalculateTangentSpace(vertexData, triCount, vertexCount);
        }

        std::vector<std::array<float, 3>> m_tangents;
        std::vector<std::array<float, 3>> m_binormals;
    };

    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory, IAssetLoadingManager& manager, std::set<XAssetInfoGeneric*>& dependencies)
            : m_stream(stream),
              m_memory(memory),
              m_script_strings(manager.GetAssetLoadingContext()->m_zone->m_script_strings),
              m_manager(manager),
              m_part_classification_state(*m_manager.GetAssetLoadingContext()->GetZoneAssetLoaderState<PartClassificationState>()),
              m_dependencies(dependencies)

        {
        }

        bool Load(XModel& xmodel)
        {
            const auto jRoot = json::parse(m_stream);
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_version").get_to(version);

            if (type != "xmodel" || version != 1u)
            {
                std::cerr << std::format("Tried to load xmodel \"{}\" but did not find expected type material of version 1\n", xmodel.name);
                return false;
            }

            try
            {
                const auto jXModel = jRoot.get<JsonXModel>();
                return CreateXModelFromJson(jXModel, xmodel);
            }
            catch (const json::exception& e)
            {
                std::cerr << std::format("Failed to parse json of xmodel: {}\n", e.what());
            }

            return false;
        }

    private:
        static void PrintError(const XModel& xmodel, const std::string& message)
        {
            std::cerr << std::format("Cannot load xmodel \"{}\": {}\n", xmodel.name, message);
        }

        static std::unique_ptr<XModelCommon> LoadModelByExtension(std::istream& stream, const std::string& extension)
        {
            if (extension == ".glb")
            {
                gltf::BinInput input;
                if (!input.ReadGltfData(stream))
                    return nullptr;

                const auto loader = gltf::Loader::CreateLoader(&input);
                return loader->Load();
            }

            if (extension == ".gltf")
            {
                gltf::TextInput input;
                if (!input.ReadGltfData(stream))
                    return nullptr;

                const auto loader = gltf::Loader::CreateLoader(&input);
                return loader->Load();
            }

            return nullptr;
        }

        static void ApplyBasePose(DObjAnimMat& baseMat, const XModelBone& bone)
        {
            baseMat.trans.x = bone.globalOffset[0];
            baseMat.trans.y = bone.globalOffset[1];
            baseMat.trans.z = bone.globalOffset[2];
            baseMat.quat.x = bone.globalRotation.x;
            baseMat.quat.y = bone.globalRotation.y;
            baseMat.quat.z = bone.globalRotation.z;
            baseMat.quat.w = bone.globalRotation.w;

            const auto quatNormSquared = Eigen::Quaternionf(baseMat.quat.w, baseMat.quat.x, baseMat.quat.y, baseMat.quat.z).squaredNorm();
            if (std::abs(quatNormSquared) < std::numeric_limits<float>::epsilon())
            {
                baseMat.quat.w = 1.0f;
                baseMat.transWeight = 2.0f;
            }
            else
            {
                baseMat.transWeight = 2.0f / quatNormSquared;
            }
        }

        static void CalculateBoneBounds(XBoneInfo& info, const unsigned boneIndex, const XModelCommon& common)
        {
            if (common.m_bone_weight_data.weights.empty())
                return;

            info.bounds[0].x = 0.0f;
            info.bounds[0].y = 0.0f;
            info.bounds[0].z = 0.0f;
            info.bounds[1].x = 0.0f;
            info.bounds[1].y = 0.0f;
            info.bounds[1].z = 0.0f;
            info.offset.x = 0.0f;
            info.offset.y = 0.0f;
            info.offset.z = 0.0f;
            info.radiusSquared = 0.0f;

            const auto vertexCount = common.m_vertex_bone_weights.size();
            for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
            {
                const auto& vertex = common.m_vertices[vertexIndex];
                const auto& vertexWeights = common.m_vertex_bone_weights[vertexIndex];
                const auto* weights = &common.m_bone_weight_data.weights[vertexWeights.weightOffset];
                for (auto weightIndex = 0u; weightIndex < vertexWeights.weightCount; weightIndex++)
                {
                    const auto& weight = weights[weightIndex];
                    if (weight.boneIndex != boneIndex)
                        continue;

                    info.bounds[0].x = std::min(info.bounds[0].x, vertex.coordinates[0]);
                    info.bounds[0].y = std::min(info.bounds[0].y, vertex.coordinates[1]);
                    info.bounds[0].z = std::min(info.bounds[0].z, vertex.coordinates[2]);
                    info.bounds[1].x = std::max(info.bounds[1].x, vertex.coordinates[0]);
                    info.bounds[1].y = std::max(info.bounds[1].y, vertex.coordinates[1]);
                    info.bounds[1].z = std::max(info.bounds[1].z, vertex.coordinates[2]);
                }
            }

            const Eigen::Vector3f minEigen(info.bounds[0].x, info.bounds[0].y, info.bounds[0].z);
            const Eigen::Vector3f maxEigen(info.bounds[1].x, info.bounds[1].y, info.bounds[1].z);
            const Eigen::Vector3f boundsCenter = (minEigen + maxEigen) * 0.5f;
            info.offset.x = boundsCenter.x();
            info.offset.y = boundsCenter.y();
            info.offset.z = boundsCenter.z();
            info.radiusSquared = Eigen::Vector3f(maxEigen - boundsCenter).squaredNorm();
        }

        bool ApplyCommonBonesToXModel(const JsonXModelLod& jLod, XModel& xmodel, unsigned lodNumber, const XModelCommon& common) const
        {
            if (common.m_bones.empty())
                return true;

            m_part_classification_state.Load(m_manager);

            const auto boneCount = common.m_bones.size();
            constexpr auto maxBones = std::numeric_limits<decltype(XModel::numBones)>::max();
            if (boneCount > maxBones)
            {
                PrintError(xmodel, std::format("Model \"{}\" for lod {} contains too many bones ({} -> max={})", jLod.file, lodNumber, boneCount, maxBones));
                return false;
            }

            xmodel.numRootBones = 0u;
            xmodel.numBones = 0u;
            for (const auto& bone : common.m_bones)
            {
                if (!bone.parentIndex)
                {
                    // Make sure root bones are at the beginning
                    assert(xmodel.numRootBones == xmodel.numBones);
                    xmodel.numRootBones++;
                }

                xmodel.numBones++;
            }

            xmodel.boneNames = m_memory.Alloc<ScriptString>(xmodel.numBones);
            xmodel.partClassification = m_memory.Alloc<unsigned char>(xmodel.numBones);
            xmodel.baseMat = m_memory.Alloc<DObjAnimMat>(xmodel.numBones);
            xmodel.boneInfo = m_memory.Alloc<XBoneInfo>(xmodel.numBones);

            if (xmodel.numBones > xmodel.numRootBones)
            {
                xmodel.parentList = m_memory.Alloc<unsigned char>(xmodel.numBones - xmodel.numRootBones);

                // For some reason Treyarch games allocate for a vec4 here. it is treated as a vec3 though?
                xmodel.trans = m_memory.Alloc<float>((xmodel.numBones - xmodel.numRootBones) * 4u);
                xmodel.quats = m_memory.Alloc<XModelQuat>(xmodel.numBones - xmodel.numRootBones);
            }
            else
            {
                xmodel.parentList = nullptr;
                xmodel.trans = nullptr;
                xmodel.quats = nullptr;
            }

            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
            {
                const auto& bone = common.m_bones[boneIndex];
                xmodel.boneNames[boneIndex] = m_script_strings.AddOrGetScriptString(bone.name);
                xmodel.partClassification[boneIndex] = static_cast<unsigned char>(m_part_classification_state.GetPartClassificationForBoneName(bone.name));

                ApplyBasePose(xmodel.baseMat[boneIndex], bone);
                CalculateBoneBounds(xmodel.boneInfo[boneIndex], boneIndex, common);

                // Other boneInfo data is filled when calculating bone bounds
                xmodel.boneInfo[boneIndex].collmap = -1;

                if (xmodel.numRootBones <= boneIndex)
                {
                    const auto nonRootIndex = boneIndex - xmodel.numRootBones;
                    const auto parentBoneIndex = static_cast<unsigned char>(bone.parentIndex.value_or(0u));
                    assert(parentBoneIndex < boneIndex);

                    xmodel.parentList[nonRootIndex] = static_cast<unsigned char>(boneIndex - parentBoneIndex);

                    auto* trans = &xmodel.trans[nonRootIndex * 3];
                    trans[0] = bone.localOffset[0];
                    trans[1] = bone.localOffset[1];
                    trans[2] = bone.localOffset[2];

                    auto& quats = xmodel.quats[nonRootIndex];
                    quats.v[0] = QuatInt16::ToInt16(bone.localRotation.x);
                    quats.v[1] = QuatInt16::ToInt16(bone.localRotation.y);
                    quats.v[2] = QuatInt16::ToInt16(bone.localRotation.z);
                    quats.v[3] = QuatInt16::ToInt16(bone.localRotation.w);
                }
            }

            return true;
        }

        [[nodiscard]] bool VerifyBones(const JsonXModelLod& jLod, const XModel& xmodel, unsigned lodNumber, const XModelCommon& common) const
        {
            // This method currently only checks names
            // This does not necessarily verify correctness entirely.
            // It is most likely enough to catch accidental errors, however.

            const auto commonBoneCount = common.m_bones.size();
            if (xmodel.numBones != commonBoneCount)
            {
                PrintError(xmodel,
                           std::format(R"(Model "{}" for lod "{}" has different bone count compared to lod 0 ({} != {}))",
                                       jLod.file,
                                       lodNumber,
                                       xmodel.numBones,
                                       commonBoneCount));
                return false;
            }

            for (auto boneIndex = 0u; boneIndex < commonBoneCount; boneIndex++)
            {
                const auto& commonBone = common.m_bones[boneIndex];

                const auto& boneName = m_script_strings[xmodel.boneNames[boneIndex]];
                if (commonBone.name != boneName)
                {
                    PrintError(xmodel,
                               std::format(R"(Model "{}" for lod "{}" has different bone names compared to lod 0 (Index {}: {} != {}))",
                                           jLod.file,
                                           lodNumber,
                                           boneIndex,
                                           boneName,
                                           commonBone.name));
                    return false;
                }
            }

            return true;
        }

        static void
            CreateVertex(GfxPackedVertex& vertex, const XModelVertex& commonVertex, const std::array<float, 3>& tangent, const std::array<float, 3>& binormal)
        {
            float tangent_[]{tangent[0], tangent[1], tangent[2]};

            vertex.xyz.x = commonVertex.coordinates[0];
            vertex.xyz.y = commonVertex.coordinates[1];
            vertex.xyz.z = commonVertex.coordinates[2];
            vertex.binormalSign = binormal[0] > 0.0f ? 1.0f : -1.0f;
            vertex.color = Common::Vec4PackGfxColor(commonVertex.color);
            vertex.texCoord = Common::Vec2PackTexCoords(commonVertex.uv);
            vertex.normal = Common::Vec3PackUnitVec(commonVertex.normal);
            vertex.tangent = Common::Vec3PackUnitVec(tangent_);
        }

        static size_t GetRigidBoneForVertex(const size_t vertexIndex, const XModelCommon& common)
        {
            return common.m_bone_weight_data.weights[common.m_vertex_bone_weights[vertexIndex].weightOffset].boneIndex;
        }

        static std::vector<std::optional<size_t>>
            GetRigidBoneIndicesForTris(const std::vector<size_t>& vertexIndices, XSurface& surface, const XModelCommon& common)
        {
            std::vector<std::optional<size_t>> rigidBoneIndexForTri;
            rigidBoneIndexForTri.reserve(surface.triCount);
            for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
            {
                const auto& tri = surface.triIndices[triIndex];
                const auto vert0Bone = GetRigidBoneForVertex(vertexIndices[tri.i[0]], common);
                const auto vert1Bone = GetRigidBoneForVertex(vertexIndices[tri.i[1]], common);
                const auto vert2Bone = GetRigidBoneForVertex(vertexIndices[tri.i[2]], common);

                const auto hasSameBone = vert0Bone == vert1Bone && vert1Bone == vert2Bone;
                if (hasSameBone)
                    rigidBoneIndexForTri.emplace_back(vert0Bone);
                else
                    rigidBoneIndexForTri.emplace_back(std::nullopt);
            }

            return rigidBoneIndexForTri;
        }

        static void ReorderRigidTrisByBoneIndex(const std::vector<size_t>& vertexIndices, XSurface& surface, const XModelCommon& common)
        {
            const auto rigidBoneIndexForTri = GetRigidBoneIndicesForTris(vertexIndices, surface, common);

            std::vector<size_t> triSortList(surface.triCount);
            std::ranges::iota(triSortList, 0);

            std::ranges::sort(triSortList,
                              [&rigidBoneIndexForTri](const size_t triIndex0, const size_t triIndex1)
                              {
                                  const auto rigidBone0 = rigidBoneIndexForTri[triIndex0];
                                  const auto rigidBone1 = rigidBoneIndexForTri[triIndex1];

                                  if (rigidBone0.has_value() != rigidBone1.has_value())
                                      return rigidBone0.has_value();
                                  if (!rigidBone0.has_value())
                                      return true;

                                  return *rigidBone0 < *rigidBone1;
                              });

            std::vector<std::remove_pointer_t<decltype(XSurface::triIndices)>> sortedTris(surface.triCount);
            for (auto i = 0u; i < surface.triCount; i++)
                memcpy(&sortedTris[i], &surface.triIndices[triSortList[i]], sizeof(std::remove_pointer_t<decltype(XSurface::triIndices)>));
            memcpy(surface.triIndices, sortedTris.data(), sizeof(std::remove_pointer_t<decltype(XSurface::triIndices)>) * surface.triCount);
        }

        static void AddBoneToXSurfacePartBits(XSurface& surface, const size_t boneIndex)
        {
            const auto partBitsIndex = boneIndex / 32u;
            const auto shiftValue = 31u - (boneIndex % 32u);
            surface.partBits[partBitsIndex] |= 1 << shiftValue;
        }

        void CreateVertListData(XSurface& surface, const std::vector<size_t>& vertexIndices, const XModelCommon& common)
        {
            ReorderRigidTrisByBoneIndex(vertexIndices, surface, common);
            const auto rigidBoneIndexForTri = GetRigidBoneIndicesForTris(vertexIndices, surface, common);

            std::vector<XRigidVertList> vertLists;

            auto currentVertexTail = 0u;
            auto currentTriTail = 0u;

            const auto vertexCount = vertexIndices.size();
            const auto triCount = static_cast<size_t>(surface.triCount);
            const auto boneCount = common.m_bones.size();
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
            {
                XRigidVertList boneVertList{};
                boneVertList.boneOffset = static_cast<uint16_t>(boneIndex * sizeof(DObjSkelMat));

                auto currentVertexHead = currentVertexTail;
                while (currentVertexHead < vertexCount && GetRigidBoneForVertex(currentVertexHead, common) == boneIndex)
                    currentVertexHead++;

                auto currentTriHead = currentTriTail;
                while (currentTriHead < triCount && rigidBoneIndexForTri[currentTriHead] && *rigidBoneIndexForTri[currentTriHead] == boneIndex)
                    currentTriHead++;

                boneVertList.vertCount = static_cast<uint16_t>(currentVertexHead - currentVertexTail);
                boneVertList.triOffset = static_cast<uint16_t>(currentTriTail);
                boneVertList.triCount = static_cast<uint16_t>(currentTriHead - currentTriTail);

                if (boneVertList.triCount > 0 || boneVertList.vertCount > 0)
                {
                    boneVertList.collisionTree = nullptr; // TODO
                    vertLists.emplace_back(boneVertList);

                    currentVertexTail = currentVertexHead;
                    currentTriTail = currentTriHead;

                    AddBoneToXSurfacePartBits(surface, boneIndex);
                }
            }

            if (!vertLists.empty())
            {
                surface.vertListCount = static_cast<unsigned char>(vertLists.size());
                surface.vertList = m_memory.Alloc<XRigidVertList>(surface.vertListCount);

                memcpy(surface.vertList, vertLists.data(), sizeof(XRigidVertList) * surface.vertListCount);
            }
        }

        void CreateVertsBlendData(XSurface& surface, const std::vector<size_t>& vertexIndices, const XModelCommon& common)
        {
            // TODO
            assert(false);
        }

        static void ReorderVerticesByWeightCount(std::vector<size_t>& vertexIndices, XSurface& surface, const XModelCommon& common)
        {
            if (common.m_bone_weight_data.weights.empty())
                return;

            const auto vertexCount = vertexIndices.size();
            std::vector<size_t> reorderLookup(vertexCount);
            std::ranges::iota(reorderLookup, 0);

            std::ranges::sort(reorderLookup,
                              [&common, &vertexIndices](const size_t& i0, const size_t& i1)
                              {
                                  const auto& weights0 = common.m_vertex_bone_weights[vertexIndices[i0]];
                                  const auto& weights1 = common.m_vertex_bone_weights[vertexIndices[i1]];

                                  if (weights0.weightCount < weights1.weightCount)
                                      return true;

                                  // If there is only one weight, make sure all vertices of the same bone follow another
                                  if (weights0.weightCount == 1)
                                  {
                                      const auto bone0 = common.m_bone_weight_data.weights[weights0.weightOffset].boneIndex;
                                      const auto bone1 = common.m_bone_weight_data.weights[weights1.weightOffset].boneIndex;
                                      return bone0 < bone1;
                                  }

                                  return false;
                              });

            for (auto triIndex = 0u; triIndex < surface.triCount; triIndex++)
            {
                auto& triIndices = surface.triIndices[triIndex];

                triIndices.i[0] = static_cast<uint16_t>(reorderLookup[triIndices.i[0]]);
                triIndices.i[1] = static_cast<uint16_t>(reorderLookup[triIndices.i[1]]);
                triIndices.i[2] = static_cast<uint16_t>(reorderLookup[triIndices.i[2]]);
            }

            for (auto& entry : reorderLookup)
                entry = vertexIndices[entry];

            vertexIndices = std::move(reorderLookup);
        }

        bool CreateXSurface(
            XSurface& surface, const XModelObject& commonObject, const XModelCommon& common, const TangentData& tangentData, unsigned& vertexOffset)
        {
            std::vector<size_t> xmodelToCommonVertexIndexLookup;
            std::unordered_map<size_t, size_t> usedVertices;

            surface.triCount = static_cast<uint16_t>(commonObject.m_faces.size());
            surface.triIndices = m_memory.Alloc<XSurfaceTri>(surface.triCount);

            for (auto faceIndex = 0u; faceIndex < surface.triCount; faceIndex++)
            {
                const auto& face = commonObject.m_faces[faceIndex];
                auto& tris = surface.triIndices[faceIndex];

                for (auto triVertIndex = 0u; triVertIndex < std::extent_v<decltype(XModelFace::vertexIndex)>; triVertIndex++)
                {
                    const auto commonVertexIndex = face.vertexIndex[triVertIndex];
                    const auto existingVertex = usedVertices.find(commonVertexIndex);
                    if (existingVertex == usedVertices.end())
                    {
                        const auto xmodelVertexIndex = xmodelToCommonVertexIndexLookup.size();
                        tris.i[triVertIndex] = static_cast<uint16_t>(xmodelVertexIndex);

                        xmodelToCommonVertexIndexLookup.emplace_back(commonVertexIndex);
                        usedVertices.emplace(commonVertexIndex, xmodelVertexIndex);
                    }
                    else
                        tris.i[triVertIndex] = static_cast<uint16_t>(existingVertex->second);
                }
            }

            ReorderVerticesByWeightCount(xmodelToCommonVertexIndexLookup, surface, common);

            surface.baseVertIndex = static_cast<uint16_t>(vertexOffset);
            surface.vertCount = static_cast<uint16_t>(xmodelToCommonVertexIndexLookup.size());
            surface.verts0 = m_memory.Alloc<GfxPackedVertex>(surface.vertCount);
            vertexOffset += surface.vertCount;

            for (auto vertexIndex = 0u; vertexIndex < surface.vertCount; vertexIndex++)
            {
                const auto commonVertexIndex = xmodelToCommonVertexIndexLookup[vertexIndex];
                const auto& commonVertex = common.m_vertices[commonVertexIndex];
                CreateVertex(surface.verts0[vertexIndex], commonVertex, tangentData.m_binormals[commonVertexIndex], tangentData.m_binormals[commonVertexIndex]);
            }

            if (!common.m_bone_weight_data.weights.empty())
            {
                // Since bone weights are sorted by weight count, the last must have the highest weight count
                const auto hasVertsBlend =
                    common.m_vertex_bone_weights[xmodelToCommonVertexIndexLookup[xmodelToCommonVertexIndexLookup.size() - 1]].weightCount > 1;
                if (!hasVertsBlend)
                    CreateVertListData(surface, xmodelToCommonVertexIndexLookup, common);
                else
                    CreateVertsBlendData(surface, xmodelToCommonVertexIndexLookup, common);
            }

            return true;
        }

        bool LoadLod(const JsonXModelLod& jLod, XModel& xmodel, unsigned lodNumber)
        {
            const auto file = m_manager.GetAssetLoadingContext()->m_raw_search_path->Open(jLod.file);
            if (!file.IsOpen())
            {
                PrintError(xmodel, std::format("Failed to open file for lod {}: \"{}\"", lodNumber, jLod.file));
                return false;
            }

            auto extension = fs::path(jLod.file).extension().string();
            utils::MakeStringLowerCase(extension);

            const auto common = LoadModelByExtension(*file.m_stream, extension);
            if (!common)
            {
                PrintError(xmodel, std::format("Failure while trying to load model for lod {}: \"{}\"", lodNumber, jLod.file));
                return false;
            }

            if (lodNumber == 0u)
            {
                if (!ApplyCommonBonesToXModel(jLod, xmodel, lodNumber, *common))
                    return false;
            }
            else
            {
                if (!VerifyBones(jLod, xmodel, lodNumber, *common))
                    return false;
            }

            auto& lodInfo = xmodel.lodInfo[lodNumber];
            lodInfo.dist = jLod.distance;
            lodInfo.surfIndex = static_cast<uint16_t>(m_surfaces.size());
            lodInfo.numsurfs = static_cast<uint16_t>(common->m_objects.size());

            std::vector<Material*> materialAssets;
            materialAssets.reserve(common->m_materials.size());
            for (const auto& commonMaterial : common->m_materials)
            {
                auto* assetInfo = m_manager.LoadDependency<AssetMaterial>(commonMaterial.name);
                if (!assetInfo)
                    return false;

                m_dependencies.emplace(assetInfo);
                materialAssets.push_back(assetInfo->Asset());
            }

            auto vertexOffset = 0u;
            TangentData tangentData;
            tangentData.CreateTangentData(*common);
            const auto surfaceCreationSuccessful =
                std::ranges::all_of(common->m_objects,
                                    [this, &common, &materialAssets, &tangentData, &vertexOffset](const XModelObject& commonObject)
                                    {
                                        XSurface surface{};
                                        if (!CreateXSurface(surface, commonObject, *common, tangentData, vertexOffset))
                                            return false;

                                        m_surfaces.emplace_back(surface);
                                        m_materials.push_back(materialAssets[commonObject.materialIndex]);
                                        return true;
                                    });

            if (!surfaceCreationSuccessful)
                return false;

            // Lod part bits are the sum of part bits of all of its surfaces
            static_assert(std::extent_v<decltype(XModelLodInfo::partBits)> == std::extent_v<decltype(XSurface::partBits)>);
            for (auto surfaceOffset = 0u; surfaceOffset < lodInfo.numsurfs; surfaceOffset++)
            {
                const auto& surface = m_surfaces[lodInfo.surfIndex + surfaceOffset];
                for (auto i = 0u; i < std::extent_v<decltype(XModelLodInfo::partBits)>; i++)
                    lodInfo.partBits[i] |= surface.partBits[i];
            }

            return true;
        }

        static void CalculateModelBounds(XModel& xmodel)
        {
            if (!xmodel.surfs)
                return;

            for (auto surfaceIndex = 0u; surfaceIndex < xmodel.lodInfo[0].numsurfs; surfaceIndex++)
            {
                const auto& surface = xmodel.surfs[surfaceIndex + xmodel.lodInfo[0].surfIndex];

                if (!surface.verts0)
                    continue;

                for (auto vertIndex = 0u; vertIndex < surface.vertCount; vertIndex++)
                {
                    const auto& vertex = surface.verts0[vertIndex];

                    xmodel.mins.x = std::min(xmodel.mins.x, vertex.xyz.v[0]);
                    xmodel.mins.y = std::min(xmodel.mins.y, vertex.xyz.v[1]);
                    xmodel.mins.z = std::min(xmodel.mins.z, vertex.xyz.v[2]);
                    xmodel.maxs.x = std::max(xmodel.maxs.x, vertex.xyz.v[0]);
                    xmodel.maxs.y = std::max(xmodel.maxs.y, vertex.xyz.v[1]);
                    xmodel.maxs.z = std::max(xmodel.maxs.z, vertex.xyz.v[2]);
                }
            }

            const auto maxX = std::max(std::abs(xmodel.mins.x), std::abs(xmodel.maxs.x));
            const auto maxY = std::max(std::abs(xmodel.mins.y), std::abs(xmodel.maxs.y));
            const auto maxZ = std::max(std::abs(xmodel.mins.z), std::abs(xmodel.maxs.z));
            xmodel.radius = Eigen::Vector3f(maxX, maxY, maxZ).norm();
        }

        bool CreateXModelFromJson(const JsonXModel& jXModel, XModel& xmodel)
        {
            auto lodNumber = 0u;
            for (const auto& jLod : jXModel.lods)
                LoadLod(jLod, xmodel, lodNumber++);
            xmodel.numLods = static_cast<uint16_t>(jXModel.lods.size());

            xmodel.numsurfs = static_cast<unsigned char>(m_surfaces.size());
            xmodel.surfs = m_memory.Alloc<XSurface>(xmodel.numsurfs);
            xmodel.materialHandles = m_memory.Alloc<Material*>(xmodel.numsurfs);
            memcpy(xmodel.surfs, m_surfaces.data(), sizeof(XSurface) * xmodel.numsurfs);
            memcpy(xmodel.materialHandles, m_materials.data(), sizeof(Material*) * xmodel.numsurfs);

            CalculateModelBounds(xmodel);

            if (jXModel.collLod && jXModel.collLod.value() >= 0)
            {
                if (static_cast<unsigned>(jXModel.collLod.value()) >= jXModel.lods.size())
                {
                    PrintError(xmodel, "Collision lod is not a valid lod");
                    return false;
                }
                xmodel.collLod = static_cast<int16_t>(jXModel.collLod.value());
            }
            else
                xmodel.collLod = -1;

            if (jXModel.physPreset)
            {
                auto* physPreset = m_manager.LoadDependency<AssetPhysPreset>(jXModel.physPreset.value());
                if (!physPreset)
                {
                    PrintError(xmodel, "Could not find phys preset");
                    return false;
                }
                m_dependencies.emplace(physPreset);
                xmodel.physPreset = physPreset->Asset();
            }
            else
            {
                xmodel.physPreset = nullptr;
            }

            if (jXModel.physConstraints)
            {
                auto* physConstraints = m_manager.LoadDependency<AssetPhysConstraints>(jXModel.physConstraints.value());
                if (!physConstraints)
                {
                    PrintError(xmodel, "Could not find phys constraints");
                    return false;
                }
                m_dependencies.emplace(physConstraints);
                xmodel.physConstraints = physConstraints->Asset();
            }
            else
            {
                xmodel.physConstraints = nullptr;
            }

            xmodel.flags = jXModel.flags;
            xmodel.lightingOriginOffset.x = jXModel.lightingOriginOffset.x;
            xmodel.lightingOriginOffset.y = jXModel.lightingOriginOffset.y;
            xmodel.lightingOriginOffset.z = jXModel.lightingOriginOffset.z;
            xmodel.lightingOriginRange = jXModel.lightingOriginRange;

            return true;
        }

        std::vector<XSurface> m_surfaces;
        std::vector<Material*> m_materials;

        std::istream& m_stream;
        MemoryManager& m_memory;
        ZoneScriptStrings& m_script_strings;
        IAssetLoadingManager& m_manager;
        PartClassificationState& m_part_classification_state;
        std::set<XAssetInfoGeneric*>& m_dependencies;
    };
} // namespace

namespace T6
{
    bool LoadXModelAsJson(
        std::istream& stream, XModel& xmodel, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        std::set<XAssetInfoGeneric*> dependenciesSet;
        JsonLoader loader(stream, *memory, *manager, dependenciesSet);

        dependencies.assign(dependenciesSet.cbegin(), dependenciesSet.cend());

        return loader.Load(xmodel);
    }
} // namespace T6
