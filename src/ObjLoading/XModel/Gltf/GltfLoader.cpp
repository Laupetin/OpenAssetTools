#include "GltfLoader.h"

#include "Internal/GltfAccessor.h"
#include "Internal/GltfBuffer.h"
#include "Internal/GltfBufferView.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <deque>
#include <exception>
#include <format>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <string>

using namespace gltf;

namespace
{
    struct AccessorsForVertex
    {
        unsigned positionAccessor;
        std::optional<unsigned> normalAccessor;
        std::optional<unsigned> colorAccessor;
        std::optional<unsigned> uvAccessor;
        std::optional<unsigned> jointsAccessor;
        std::optional<unsigned> weightsAccessor;

        friend bool operator==(const AccessorsForVertex& lhs, const AccessorsForVertex& rhs)
        {
            return lhs.positionAccessor == rhs.positionAccessor && lhs.normalAccessor == rhs.normalAccessor && lhs.colorAccessor == rhs.colorAccessor
                   && lhs.uvAccessor == rhs.uvAccessor && lhs.jointsAccessor == rhs.jointsAccessor && lhs.weightsAccessor == rhs.weightsAccessor;
        }

        friend bool operator!=(const AccessorsForVertex& lhs, const AccessorsForVertex& rhs)
        {
            return !(lhs == rhs);
        }
    };
} // namespace

template<> struct std::hash<AccessorsForVertex>
{
    std::size_t operator()(const AccessorsForVertex& v) const noexcept
    {
        std::size_t seed = 0x7E42C0E6;
        seed ^= (seed << 6) + (seed >> 2) + 0x47B15429 + static_cast<std::size_t>(v.positionAccessor);
        seed ^= (seed << 6) + (seed >> 2) + 0x66847B5C + std::hash<std::optional<unsigned>>()(v.normalAccessor);
        seed ^= (seed << 6) + (seed >> 2) + 0x77399D60 + std::hash<std::optional<unsigned>>()(v.colorAccessor);
        seed ^= (seed << 6) + (seed >> 2) + 0x477AF9AB + std::hash<std::optional<unsigned>>()(v.uvAccessor);
        seed ^= (seed << 6) + (seed >> 2) + 0x4421B4D9 + std::hash<std::optional<unsigned>>()(v.jointsAccessor);
        seed ^= (seed << 6) + (seed >> 2) + 0x13C2EBA1 + std::hash<std::optional<unsigned>>()(v.weightsAccessor);
        return seed;
    }
};

namespace
{
    class GltfLoadException final : std::exception
    {
    public:
        explicit GltfLoadException(std::string message)
            : m_message(std::move(message))
        {
        }

        [[nodiscard]] const std::string& Str() const
        {
            return m_message;
        }

        [[nodiscard]] const char* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    struct ObjectToLoad
    {
        unsigned meshIndex;
        std::optional<unsigned> skinIndex;

        ObjectToLoad(const unsigned meshIndex, const std::optional<unsigned> skinIndex)
            : meshIndex(meshIndex),
              skinIndex(skinIndex)
        {
        }
    };

    class GltfLoaderImpl final : public Loader
    {
    public:
        explicit GltfLoaderImpl(const Input* input)
            : m_input(input)
        {
        }

        static std::vector<unsigned> GetRootNodes(const JsonRoot& jRoot)
        {
            if (!jRoot.nodes || jRoot.nodes->empty())
                return {};

            const auto nodeCount = jRoot.nodes->size();
            std::vector<unsigned> rootNodes;
            std::vector<bool> isChild(nodeCount);

            for (const auto& node : jRoot.nodes.value())
            {
                if (!node.children)
                    continue;

                for (const auto childIndex : node.children.value())
                {
                    if (childIndex >= nodeCount)
                        throw GltfLoadException("Illegal child index");

                    if (isChild[childIndex])
                        throw GltfLoadException("Node hierarchy is not a set of disjoint strict trees");

                    isChild[childIndex] = true;
                }
            }

            for (auto nodeIndex = 0u; nodeIndex < nodeCount; nodeIndex++)
            {
                if (!isChild[nodeIndex])
                    rootNodes.emplace_back(nodeIndex);
            }

            return rootNodes;
        }

        void TraverseNodes(const JsonRoot& jRoot)
        {
            // Make sure there are any nodes to traverse
            if (!jRoot.nodes || jRoot.nodes->empty())
                return;

            std::deque<unsigned> nodeQueue;
            std::vector<unsigned> rootNodes = GetRootNodes(jRoot);

            for (const auto rootNode : rootNodes)
                nodeQueue.emplace_back(rootNode);

            while (!nodeQueue.empty())
            {
                const auto& node = jRoot.nodes.value()[nodeQueue.front()];
                nodeQueue.pop_front();

                if (node.children)
                {
                    for (const auto childIndex : *node.children)
                        nodeQueue.emplace_back(childIndex);
                }

                if (node.mesh)
                    m_load_objects.emplace_back(*node.mesh, node.skin);
            }
        }

        std::optional<Accessor*> GetAccessorForIndex(const char* attributeName,
                                                     const std::optional<unsigned> index,
                                                     std::initializer_list<JsonAccessorType> allowedAccessorTypes,
                                                     std::initializer_list<JsonAccessorComponentType> allowedAccessorComponentTypes) const
        {
            if (!index)
                return std::nullopt;

            if (*index > m_accessors.size())
                throw GltfLoadException(std::format("Index for {} accessor out of bounds", attributeName));

            auto* accessor = m_accessors[*index].get();

            const auto maybeType = accessor->GetType();
            if (maybeType)
            {
                if (std::ranges::find(allowedAccessorTypes, *maybeType) == allowedAccessorTypes.end())
                    throw GltfLoadException(std::format("Accessor for {} has unsupported type {}", attributeName, static_cast<unsigned>(*maybeType)));
            }

            const auto maybeComponentType = accessor->GetComponentType();
            if (maybeComponentType)
            {
                if (std::ranges::find(allowedAccessorComponentTypes, *maybeComponentType) == allowedAccessorComponentTypes.end())
                    throw GltfLoadException(
                        std::format("Accessor for {} has unsupported component type {}", attributeName, static_cast<unsigned>(*maybeComponentType)));
            }

            return accessor;
        }

        static void VerifyAccessorVertexCount(const char* accessorType, const Accessor* accessor, const size_t vertexCount)
        {
            if (accessor->GetCount() != vertexCount)
                throw GltfLoadException(std::format("Element count of {} accessor does not match expected vertex count of {}", accessorType, vertexCount));
        }

        unsigned CreateVertices(XModelCommon& common, const AccessorsForVertex& accessorsForVertex)
        {
            // clang-format off
            auto* positionAccessor = GetAccessorForIndex(
                "POSITION",
                accessorsForVertex.positionAccessor, 
                {JsonAccessorType::VEC3}, 
                {JsonAccessorComponentType::FLOAT}
            ).value_or(nullptr);
            // clang-format on

            const auto vertexCount = positionAccessor->GetCount();
            NullAccessor nullAccessor(vertexCount);
            OnesAccessor onesAccessor(vertexCount);

            // clang-format off
            auto* normalAccessor = GetAccessorForIndex(
                "NORMAL",
                accessorsForVertex.normalAccessor, 
                {JsonAccessorType::VEC3}, 
                {JsonAccessorComponentType::FLOAT}
            ).value_or(&nullAccessor);
            VerifyAccessorVertexCount("NORMAL", normalAccessor, vertexCount);

            auto* uvAccessor = GetAccessorForIndex(
                "TEXCOORD_0",
                accessorsForVertex.uvAccessor,
                {JsonAccessorType::VEC2},
                {JsonAccessorComponentType::FLOAT, JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT}
            ).value_or(&nullAccessor);
            VerifyAccessorVertexCount("TEXCOORD_0", uvAccessor, vertexCount);

            auto* colorAccessor = GetAccessorForIndex(
                "COLOR_0",
                accessorsForVertex.colorAccessor,
                {JsonAccessorType::VEC3, JsonAccessorType::VEC4},
                {JsonAccessorComponentType::FLOAT, JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT}
            ).value_or(&onesAccessor);
            VerifyAccessorVertexCount("COLOR_0", colorAccessor, vertexCount);

            auto* jointsAccessor = GetAccessorForIndex(
                "JOINTS_0",
                accessorsForVertex.jointsAccessor,
                {JsonAccessorType::VEC4},
                {JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT}
            ).value_or(&nullAccessor);
            VerifyAccessorVertexCount("JOINTS_0", jointsAccessor, vertexCount);

            auto* weightsAccessor = GetAccessorForIndex(
                "WEIGHTS_0",
                accessorsForVertex.weightsAccessor,
                {JsonAccessorType::VEC4},
                {JsonAccessorComponentType::FLOAT, JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT}
            ).value_or(&nullAccessor);
            VerifyAccessorVertexCount("WEIGHTS_0", weightsAccessor, vertexCount);
            // clang-format on

            const auto vertexOffset = static_cast<unsigned>(common.m_vertices.size());
            common.m_vertices.reserve(vertexOffset + vertexCount);
            common.m_vertex_bone_weights.reserve(vertexOffset + vertexCount);
            for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
            {
                XModelVertex vertex;

                unsigned joints[4];
                float weights[4];

                float coordinates[3];
                float normal[3];
                if (!positionAccessor->GetFloatVec3(vertexIndex, coordinates) || !normalAccessor->GetFloatVec3(vertexIndex, normal)
                    || !colorAccessor->GetFloatVec4(vertexIndex, vertex.color) || !uvAccessor->GetFloatVec2(vertexIndex, vertex.uv)
                    || !jointsAccessor->GetUnsignedVec4(vertexIndex, joints) || !weightsAccessor->GetFloatVec4(vertexIndex, weights))
                {
                    return false;
                }

                vertex.coordinates[0] = coordinates[0];
                vertex.coordinates[1] = -coordinates[2];
                vertex.coordinates[2] = coordinates[1];
                vertex.normal[0] = normal[0];
                vertex.normal[1] = -normal[2];
                vertex.normal[2] = normal[1];

                common.m_vertices.emplace_back(vertex);

                XModelVertexBoneWeights vertexWeights{.weightOffset = static_cast<unsigned>(common.m_bone_weight_data.weights.size()), .weightCount = 0u};
                for (auto i = 0u; i < std::extent_v<decltype(joints)>; i++)
                {
                    if (std::abs(weights[i]) < std::numeric_limits<float>::epsilon())
                        continue;

                    common.m_bone_weight_data.weights.emplace_back(joints[i], weights[i]);
                    vertexWeights.weightCount++;
                }

                common.m_vertex_bone_weights.emplace_back(vertexWeights);
            }

            m_vertex_offset_for_accessors.emplace(accessorsForVertex, vertexOffset);

            return vertexOffset;
        }

        bool ConvertObject(const JsonRoot& jRoot, const JsonMeshPrimitives& primitives, XModelCommon& common, XModelObject& object)
        {
            if (!primitives.indices)
                throw GltfLoadException("Requires primitives indices");
            if (!primitives.material)
                throw GltfLoadException("Requires primitives material");
            if (primitives.mode.value_or(JsonMeshPrimitivesMode::TRIANGLES) != JsonMeshPrimitivesMode::TRIANGLES)
                throw GltfLoadException("Only triangles are supported");
            if (!primitives.attributes.POSITION)
                throw GltfLoadException("Requires primitives attribute POSITION");

            AccessorsForVertex accessorsForVertex{
                .positionAccessor = *primitives.attributes.POSITION,
                .normalAccessor = primitives.attributes.NORMAL,
                .colorAccessor = primitives.attributes.COLOR_0,
                .uvAccessor = primitives.attributes.TEXCOORD_0,
                .jointsAccessor = primitives.attributes.JOINTS_0,
                .weightsAccessor = primitives.attributes.WEIGHTS_0,
            };

            const auto existingVertices = m_vertex_offset_for_accessors.find(accessorsForVertex);
            const auto vertexOffset =
                existingVertices == m_vertex_offset_for_accessors.end() ? CreateVertices(common, accessorsForVertex) : existingVertices->second;

            // clang-format off
            const auto* indexAccessor = GetAccessorForIndex(
                "INDICES",
                primitives.indices, 
                {JsonAccessorType::SCALAR}, 
                {JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT, JsonAccessorComponentType::UNSIGNED_INT}
            ).value_or(nullptr);
            // clang-format on

            const auto indexCount = indexAccessor->GetCount();
            if (indexCount % 3 != 0)
                throw GltfLoadException("Index count must be dividable by 3 for triangles");
            const auto faceCount = indexCount / 3u;
            object.m_faces.reserve(faceCount);

            for (auto faceIndex = 0u; faceIndex < faceCount; faceIndex++)
            {
                unsigned indices[3];
                if (!indexAccessor->GetUnsigned(faceIndex * 3u + 0u, indices[0]) || !indexAccessor->GetUnsigned(faceIndex * 3u + 1u, indices[1])
                    || !indexAccessor->GetUnsigned(faceIndex * 3u + 2u, indices[2]))
                {
                    return false;
                }

                object.m_faces.emplace_back(XModelFace{
                    vertexOffset + indices[2],
                    vertexOffset + indices[1],
                    vertexOffset + indices[0],
                });
            }

            if (!jRoot.materials || *primitives.material >= jRoot.materials->size())
                throw GltfLoadException("Invalid material index");

            object.materialIndex = static_cast<int>(*primitives.material);

            return true;
        }

        static std::optional<unsigned> GetRootNodeForSkin(const JsonRoot& jRoot, const JsonSkin& skin)
        {
            if (!jRoot.nodes || skin.joints.empty())
                return std::nullopt;

            const auto jointCount = skin.joints.size();
            auto rootCount = jointCount;
            std::vector<bool> isRoot(jointCount, true);

            for (const auto joint : skin.joints)
            {
                if (jRoot.nodes->size() <= joint)
                    throw GltfLoadException("Invalid joint index");

                const auto& node = jRoot.nodes.value()[joint];
                if (node.children)
                {
                    for (const auto child : *node.children)
                    {
                        const auto foundChildJoint = std::ranges::find(skin.joints, child);
                        if (foundChildJoint != skin.joints.end())
                        {
                            const auto foundChildJointIndex = std::distance(skin.joints.begin(), foundChildJoint);
                            if (isRoot[foundChildJointIndex])
                            {
                                isRoot[foundChildJointIndex] = false;
                                rootCount--;
                            }
                        }
                    }
                }
            }

            if (rootCount != 1)
                throw GltfLoadException("Skins must have exactly one common root node");

            for (auto index = 0u; index < jointCount; index++)
            {
                if (isRoot[index])
                    return skin.joints[index];
            }

            return std::nullopt;
        }

        static void ApplyNodeMatrixTRS(XModelBone& bone, const JsonNode& node)
        {
            const auto matrix = Eigen::Matrix4f({
                {(*node.matrix)[0], (*node.matrix)[4], (*node.matrix)[8],  (*node.matrix)[12]},
                {(*node.matrix)[1], (*node.matrix)[5], (*node.matrix)[9],  (*node.matrix)[13]},
                {(*node.matrix)[2], (*node.matrix)[6], (*node.matrix)[10], (*node.matrix)[14]},
                {(*node.matrix)[3], (*node.matrix)[7], (*node.matrix)[11], (*node.matrix)[15]}
            });
            Eigen::Affine3f transform(matrix);

            const auto translation = transform.translation();
            bone.localOffset[0] = translation.x();
            bone.localOffset[1] = -translation.z();
            bone.localOffset[2] = translation.y();

            const auto rotation = transform.rotation();
            const auto rotationQuat = Eigen::Quaternionf(rotation);
            bone.localRotation.x = rotationQuat.x();
            bone.localRotation.y = -rotationQuat.z();
            bone.localRotation.z = rotationQuat.y();
            bone.localRotation.w = rotationQuat.w();

            bone.scale[0] = matrix.block<3, 1>(0, 0).norm();
            bone.scale[1] = matrix.block<3, 1>(0, 1).norm();
            bone.scale[2] = matrix.block<3, 1>(0, 2).norm();
        }

        static void ApplyNodeSeparateTRS(XModelBone& bone, const JsonNode& node)
        {
            if (node.translation)
            {
                bone.localOffset[0] = (*node.translation)[0];
                bone.localOffset[1] = -(*node.translation)[2];
                bone.localOffset[2] = (*node.translation)[1];
            }
            else
            {
                bone.localOffset[0] = 0.0f;
                bone.localOffset[1] = 0.0f;
                bone.localOffset[2] = 0.0f;
            }

            if (node.rotation)
            {
                bone.localRotation.x = (*node.rotation)[0];
                bone.localRotation.y = -(*node.rotation)[2];
                bone.localRotation.z = (*node.rotation)[1];
                bone.localRotation.w = (*node.rotation)[3];
            }
            else
            {
                bone.localRotation.x = 0.0f;
                bone.localRotation.y = 0.0f;
                bone.localRotation.z = 0.0f;
                bone.localRotation.w = 1.0f;
            }

            if (node.scale)
            {
                bone.scale[0] = (*node.scale)[0];
                bone.scale[1] = (*node.scale)[1];
                bone.scale[2] = (*node.scale)[2];
            }
            else
            {
                bone.scale[0] = 1.0f;
                bone.scale[1] = 1.0f;
                bone.scale[2] = 1.0f;
            }
        }

        static bool ConvertJoint(const JsonRoot& jRoot,
                                 const JsonSkin& skin,
                                 XModelCommon& common,
                                 const unsigned skinBoneOffset,
                                 const unsigned nodeIndex,
                                 const std::optional<unsigned> parentIndex,
                                 const float (&parentOffset)[3],
                                 const XModelQuaternion& parentRotation,
                                 const float (&parentScale)[3])
        {
            if (!jRoot.nodes || nodeIndex >= jRoot.nodes->size())
                return false;

            const auto& node = jRoot.nodes.value()[nodeIndex];
            const auto boneInSkin = std::ranges::find(skin.joints, nodeIndex);
            if (boneInSkin == skin.joints.end())
                throw GltfLoadException("Bone node is not part of skin");
            const auto boneIndexInSkin = static_cast<unsigned>(std::distance(skin.joints.begin(), boneInSkin));

            const auto commonBoneOffset = skinBoneOffset + boneIndexInSkin;
            auto& bone = common.m_bones[commonBoneOffset];
            bone.name = node.name.value_or(std::string());
            bone.parentIndex = parentIndex;

            if (node.matrix)
                ApplyNodeMatrixTRS(bone, node);
            else
                ApplyNodeSeparateTRS(bone, node);

            bone.scale[0] *= parentScale[0];
            bone.scale[1] *= parentScale[1];
            bone.scale[2] *= parentScale[2];

            const auto localRotationEigen = Eigen::Quaternionf(bone.localRotation.w, bone.localRotation.x, bone.localRotation.y, bone.localRotation.z);
            const auto parentRotationEigen = Eigen::Quaternionf(parentRotation.w, parentRotation.x, parentRotation.y, parentRotation.z);
            const auto globalRotationEigen = (parentRotationEigen * localRotationEigen).normalized();

            const Eigen::Vector3f localTranslationEigen(bone.localOffset[0], bone.localOffset[1], bone.localOffset[2]);
            const Eigen::Vector3f parentTranslationEigen(parentOffset[0], parentOffset[1], parentOffset[2]);
            const auto globalTranslationEigen = (parentRotationEigen * localTranslationEigen) + parentTranslationEigen;
            bone.globalOffset[0] = globalTranslationEigen.x();
            bone.globalOffset[1] = globalTranslationEigen.y();
            bone.globalOffset[2] = globalTranslationEigen.z();

            bone.globalRotation.x = globalRotationEigen.x();
            bone.globalRotation.y = globalRotationEigen.y();
            bone.globalRotation.z = globalRotationEigen.z();
            bone.globalRotation.w = globalRotationEigen.w();

            if (node.children)
            {
                for (const auto childIndex : *node.children)
                {
                    if (!ConvertJoint(jRoot, skin, common, skinBoneOffset, childIndex, commonBoneOffset, bone.globalOffset, bone.globalRotation, bone.scale))
                        return false;
                }
            }

            return true;
        }

        static bool ConvertSkin(const JsonRoot& jRoot, const JsonSkin& skin, XModelCommon& common)
        {
            if (skin.joints.empty())
                return true;
            if (!jRoot.nodes)
                return false;

            const auto rootNode = GetRootNodeForSkin(jRoot, skin).value_or(skin.joints[0]);
            const auto skinBoneOffset = static_cast<unsigned>(common.m_bones.size());
            common.m_bones.resize(skinBoneOffset + skin.joints.size());

            constexpr float defaultTranslation[3]{0.0f, 0.0f, 0.0f};
            constexpr XModelQuaternion defaultRotation{.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
            constexpr float defaultScale[3]{1.0f, 1.0f, 1.0f};

            return ConvertJoint(jRoot, skin, common, skinBoneOffset, rootNode, std::nullopt, defaultTranslation, defaultRotation, defaultScale);
        }

        void ConvertObjects(const JsonRoot& jRoot, XModelCommon& common)
        {
            if (!jRoot.meshes)
                return;

            std::optional<unsigned> alreadyLoadedSkinIndex;

            for (const auto& loadObject : m_load_objects)
            {
                if (loadObject.skinIndex && jRoot.skins)
                {
                    if (alreadyLoadedSkinIndex)
                    {
                        if (*alreadyLoadedSkinIndex != *loadObject.skinIndex)
                            throw GltfLoadException("Only scenes with at most one skin are supported");

                        // Do not load already loaded skin
                    }
                    else
                    {
                        const auto& skin = jRoot.skins.value()[*loadObject.skinIndex];
                        if (!ConvertSkin(jRoot, skin, common))
                            return;

                        alreadyLoadedSkinIndex = *loadObject.skinIndex;
                    }
                }

                const auto& mesh = jRoot.meshes.value()[loadObject.meshIndex];

                common.m_objects.reserve(common.m_objects.size() + mesh.primitives.size());
                for (const auto& primitives : mesh.primitives)
                {
                    XModelObject object;
                    if (!ConvertObject(jRoot, primitives, common, object))
                        return;

                    common.m_objects.emplace_back(std::move(object));
                }
            }
        }

        static void ConvertMaterials(const JsonRoot& jRoot, XModelCommon& common)
        {
            if (!jRoot.materials)
                return;

            common.m_materials.reserve(jRoot.materials->size());
            for (const auto& jMaterial : *jRoot.materials)
            {
                XModelMaterial material;
                material.ApplyDefaults();

                if (jMaterial.name)
                    material.name = *jMaterial.name;
                else
                    throw GltfLoadException("Materials must have a name");

                common.m_materials.emplace_back(std::move(material));
            }
        }

        void CreateBuffers(const JsonRoot& jRoot)
        {
            if (!jRoot.buffers)
                return;

            m_buffers.reserve(jRoot.buffers->size());
            for (const auto& jBuffer : *jRoot.buffers)
            {
                if (!jBuffer.uri)
                {
                    const void* embeddedBufferPtr = nullptr;
                    size_t embeddedBufferSize = 0u;
                    if (!m_input->GetEmbeddedBuffer(embeddedBufferPtr, embeddedBufferSize) || embeddedBufferSize == 0u)
                        throw GltfLoadException("Buffer tried to access embedded data when there is none");

                    m_buffers.emplace_back(std::make_unique<EmbeddedBuffer>(embeddedBufferPtr, embeddedBufferSize));
                }
                else if (DataUriBuffer::IsDataUri(*jBuffer.uri))
                {
                    auto dataUriBuffer = std::make_unique<DataUriBuffer>();
                    if (!dataUriBuffer->ReadDataFromUri(*jBuffer.uri))
                        throw GltfLoadException("Buffer has invalid data uri");

                    m_buffers.emplace_back(std::move(dataUriBuffer));
                }
                else
                {
                    throw GltfLoadException("File buffers are not supported");
                }
            }
        }

        void CreateBufferViews(const JsonRoot& jRoot)
        {
            if (!jRoot.bufferViews)
                return;

            m_buffer_views.reserve(jRoot.bufferViews->size());
            for (const auto& jBufferView : *jRoot.bufferViews)
            {
                if (jBufferView.buffer >= m_buffers.size())
                    throw GltfLoadException("Buffer view references invalid buffer");

                const auto* buffer = m_buffers[jBufferView.buffer].get();
                const auto offset = jBufferView.byteOffset.value_or(0u);
                const auto length = jBufferView.byteLength;
                const auto stride = jBufferView.byteStride.value_or(0u);

                if (offset + length > buffer->GetSize())
                    throw GltfLoadException("Buffer view is defined larger as underlying buffer");

                m_buffer_views.emplace_back(std::make_unique<BufferView>(buffer, offset, length, stride));
            }
        }

        void CreateAccessors(const JsonRoot& jRoot)
        {
            if (!jRoot.accessors)
                return;

            m_accessors.reserve(jRoot.accessors->size());
            for (const auto& jAccessor : *jRoot.accessors)
            {
                if (!jAccessor.bufferView)
                {
                    m_accessors.emplace_back(std::make_unique<NullAccessor>(jAccessor.count));
                    continue;
                }

                if (*jAccessor.bufferView >= m_buffer_views.size())
                    throw GltfLoadException("Accessor references invalid buffer view");

                const auto* bufferView = m_buffer_views[*jAccessor.bufferView].get();
                const auto byteOffset = jAccessor.byteOffset.value_or(0u);
                if (jAccessor.componentType == JsonAccessorComponentType::FLOAT)
                    m_accessors.emplace_back(std::make_unique<FloatAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_BYTE)
                    m_accessors.emplace_back(std::make_unique<UnsignedByteAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_SHORT)
                    m_accessors.emplace_back(std::make_unique<UnsignedShortAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_INT)
                    m_accessors.emplace_back(std::make_unique<UnsignedIntAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else
                    throw GltfLoadException(std::format("Accessor has unsupported component type {}", static_cast<unsigned>(jAccessor.componentType)));
            }
        }

        std::unique_ptr<XModelCommon> Load() override
        {
            JsonRoot jRoot;
            try
            {
                jRoot = m_input->GetJson().get<JsonRoot>();
            }
            catch (const nlohmann::json::exception& e)
            {
                std::cerr << std::format("Failed to parse GLTF JSON: {}\n", e.what());
                return nullptr;
            }

            try
            {
                CreateBuffers(jRoot);
                CreateBufferViews(jRoot);
                CreateAccessors(jRoot);

                TraverseNodes(jRoot);

                auto common = std::make_unique<XModelCommon>();
                ConvertObjects(jRoot, *common);
                ConvertMaterials(jRoot, *common);

                return common;
            }
            catch (const GltfLoadException& e)
            {
                std::cerr << std::format("Failed to load GLTF: {}\n", e.Str());
                return nullptr;
            }
        }

    private:
        const Input* m_input;
        std::vector<ObjectToLoad> m_load_objects;
        std::unordered_map<AccessorsForVertex, unsigned> m_vertex_offset_for_accessors;
        std::vector<std::unique_ptr<Accessor>> m_accessors;
        std::vector<std::unique_ptr<BufferView>> m_buffer_views;
        std::vector<std::unique_ptr<Buffer>> m_buffers;
    };
} // namespace

std::unique_ptr<Loader> Loader::CreateLoader(const Input* input)
{
    return std::make_unique<GltfLoaderImpl>(input);
}
