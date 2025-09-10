#include "GltfWriter.h"

#include "GitVersion.h"
#include "XModel/Gltf/GltfConstants.h"
#include "XModel/Gltf/JsonGltf.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <algorithm>
#include <format>
#include <numbers>

using namespace gltf;
using namespace nlohmann;

namespace
{
    constexpr auto GLTF_GENERATOR = "OpenAssetTools " GIT_VERSION;

    struct GltfVertex
    {
        float coordinates[3];
        float normal[3];
        float uv[2];
    };

    void LhcToRhcCoordinates(float (&coords)[3])
    {
        const float two[3]{coords[0], coords[1], coords[2]};

        coords[0] = two[0];
        coords[1] = two[2];
        coords[2] = -two[1];
    }

    void LhcToRhcQuaternion(float (&quat)[4])
    {
        Eigen::Quaternionf eigenQuat(quat[3], quat[0], quat[1], quat[2]);
        const Eigen::Quaternionf eigenRotationQuat(Eigen::AngleAxisf(-std::numbers::pi_v<float> / 2.f, Eigen::Vector3f::UnitX()));

        eigenQuat = eigenRotationQuat * eigenQuat;

        quat[0] = eigenQuat.x();
        quat[1] = eigenQuat.y();
        quat[2] = eigenQuat.z();
        quat[3] = eigenQuat.w();
    }

    void LhcToRhcIndices(unsigned short* indices)
    {
        const unsigned short two[3]{indices[0], indices[1], indices[2]};

        indices[0] = two[2];
        indices[1] = two[1];
        indices[2] = two[0];
    }

    void LhcToRhcMatrix(Eigen::Matrix4f& matrix)
    {
        const Eigen::Matrix4f convertMatrix({
            {1.0, 0.0,  0.0, 0.0},
            {0.0, 0.0,  1.0, 0.0},
            {0.0, -1.0, 0.0, 0.0},
            {0.0, 0.0,  0.0, 1.0}
        });

        const auto result = convertMatrix * matrix;
        matrix = result;
    }

    class GltfWriterImpl final : public gltf::Writer
    {
    public:
        GltfWriterImpl(const Output* output, std::string gameName, std::string zoneName)
            : m_output(output),
              m_game_name(std::move(gameName)),
              m_zone_name(std::move(zoneName))
        {
        }

        void Write(const XModelCommon& xmodel) override
        {
            JsonRoot gltf;
            std::vector<uint8_t> bufferData;

            CreateJsonAsset(gltf.asset);
            CreateSkeletonNodes(gltf, xmodel);
            CreateMeshNodes(gltf, xmodel);
            CreateRootNode(gltf, xmodel);
            CreateMaterials(gltf, xmodel);
            CreateBufferViews(gltf, xmodel);
            CreateAccessors(gltf, xmodel);
            CreateSkin(gltf, xmodel);
            CreateMeshes(gltf, xmodel);
            CreateScene(gltf, xmodel);
            FillBufferData(gltf, xmodel, bufferData);
            CreateBuffer(gltf, xmodel, bufferData);

            const json jRoot = gltf;
            m_output->EmitJson(jRoot);

            if (!bufferData.empty())
                m_output->EmitBuffer(bufferData.data(), bufferData.size());

            m_output->Finalize();
        }

    private:
        static void CreateJsonAsset(JsonAsset& asset)
        {
            asset.version = GLTF_VERSION_STRING;
            asset.generator = GLTF_GENERATOR;
        }

        void CreateMeshNodes(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            m_first_mesh_node = static_cast<unsigned>(gltf.nodes->size());

            auto meshIndex = 0u;
            for (const auto& object : xmodel.m_objects)
            {
                JsonNode meshNode;

                if (!object.name.empty())
                    meshNode.name = object.name;

                meshNode.mesh = meshIndex++;

                // Only add skin if the model has bones
                if (!xmodel.m_bones.empty())
                {
                    // We only have one skin
                    meshNode.skin = 0u;
                }

                gltf.nodes->emplace_back(std::move(meshNode));
            }

            // If we only have one mesh and no bones we don't need a dedicated root node
            if (xmodel.m_bones.empty() && xmodel.m_objects.size() == 1)
                m_root_node = m_first_mesh_node;
        }

        void CreateRootNode(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            JsonNode rootNode;

            // If we have at most one mesh and no bones we don't need a dedicated root node
            if (xmodel.m_bones.empty() && xmodel.m_objects.size() <= 1)
                return;

            if (!xmodel.m_name.empty())
                rootNode.name = std::format("{}_skel", xmodel.m_name);

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            rootNode.children.emplace();

            const auto meshCount = xmodel.m_objects.size();
            for (auto meshIndex = 0u; meshIndex < meshCount; meshIndex++)
                rootNode.children->push_back(m_first_mesh_node + meshIndex);

            if (!xmodel.m_bones.empty())
                rootNode.children->push_back(m_first_bone_node);

            m_root_node = static_cast<unsigned>(gltf.nodes->size());
            gltf.nodes->emplace_back(std::move(rootNode));
        }

        void CreateMeshes(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.meshes.has_value())
                gltf.meshes.emplace();

            const auto hasBoneWeightData = !xmodel.m_bone_weight_data.weights.empty();

            auto objectIndex = 0u;
            for (const auto& object : xmodel.m_objects)
            {
                JsonMesh mesh;
                JsonMeshPrimitives primitives;

                primitives.material = object.materialIndex;

                primitives.attributes.POSITION = m_position_accessor;
                primitives.attributes.NORMAL = m_normal_accessor;
                primitives.attributes.TEXCOORD_0 = m_uv_accessor;

                if (hasBoneWeightData)
                {
                    primitives.attributes.JOINTS_0 = m_joints_accessor;
                    primitives.attributes.WEIGHTS_0 = m_weights_accessor;
                }

                primitives.mode = JsonMeshPrimitivesMode::TRIANGLES;
                primitives.indices = m_first_index_accessor + objectIndex;

                mesh.primitives.emplace_back(primitives);
                objectIndex++;
                gltf.meshes->emplace_back(std::move(mesh));
            }
        }

        static void CreateMaterials(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.materials.has_value())
                gltf.materials.emplace();

            for (const auto& modelMaterial : xmodel.m_materials)
            {
                JsonMaterial material;

                material.name = modelMaterial.name;

                if (!modelMaterial.colorMapName.empty())
                {
                    material.pbrMetallicRoughness.emplace();
                    material.pbrMetallicRoughness->baseColorTexture.emplace();

                    material.pbrMetallicRoughness->baseColorTexture->index = CreateTexture(gltf, modelMaterial.colorMapName);
                    material.pbrMetallicRoughness->metallicFactor = 0.0f;
                }

                if (!modelMaterial.normalMapName.empty())
                    material.normalTexture.emplace().index = CreateTexture(gltf, modelMaterial.normalMapName);

                material.doubleSided = true;
                gltf.materials->emplace_back(material);
            }
        }

        static unsigned CreateTexture(JsonRoot& gltf, const std::string& textureName)
        {
            if (!gltf.textures.has_value())
                gltf.textures.emplace();
            if (!gltf.images.has_value())
                gltf.images.emplace();

            auto uri = std::format("../images/{}.dds", textureName);

            auto existingTexIndex = 0u;
            for (const auto& existingTex : gltf.textures.value())
            {
                const auto& existingImage = gltf.images.value()[existingTex.source];

                if (existingImage.uri == uri)
                    return existingTexIndex;

                existingTexIndex++;
            }

            JsonImage image;
            image.uri = std::move(uri);
            const auto imageIndex = static_cast<unsigned>(gltf.images->size());
            gltf.images->emplace_back(std::move(image));

            JsonTexture texture;
            texture.source = imageIndex;
            const auto textureIndex = static_cast<unsigned>(gltf.textures->size());
            gltf.textures->emplace_back(texture);

            return textureIndex;
        }

        void CreateSkeletonNodes(JsonRoot& gltf, const XModelCommon& common)
        {
            if (common.m_bones.empty())
                return;

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            const auto boneCount = common.m_bones.size();
            m_first_bone_node = static_cast<unsigned>(gltf.nodes->size());
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
            {
                JsonNode boneNode;
                const auto& bone = common.m_bones[boneIndex];

                float globalTranslationData[3]{bone.globalOffset[0], bone.globalOffset[1], bone.globalOffset[2]};
                LhcToRhcCoordinates(globalTranslationData);
                Eigen::Vector3f translation(globalTranslationData[0], globalTranslationData[1], globalTranslationData[2]);

                float globalRotationData[4]{bone.globalRotation.x, bone.globalRotation.y, bone.globalRotation.z, bone.globalRotation.w};
                LhcToRhcQuaternion(globalRotationData);
                Eigen::Quaternionf rotation(globalRotationData[3], globalRotationData[0], globalRotationData[1], globalRotationData[2]);

                if (bone.parentIndex)
                {
                    const auto& parentBone = common.m_bones[*bone.parentIndex];

                    float parentGlobalTranslationData[3]{parentBone.globalOffset[0], parentBone.globalOffset[1], parentBone.globalOffset[2]};
                    LhcToRhcCoordinates(parentGlobalTranslationData);
                    const Eigen::Vector3f parentTranslation(parentGlobalTranslationData[0], parentGlobalTranslationData[1], parentGlobalTranslationData[2]);

                    float parentGlobalRotationData[4]{
                        parentBone.globalRotation.x, parentBone.globalRotation.y, parentBone.globalRotation.z, parentBone.globalRotation.w};
                    LhcToRhcQuaternion(parentGlobalRotationData);
                    const Eigen::Quaternionf parentRotation(
                        parentGlobalRotationData[3], parentGlobalRotationData[0], parentGlobalRotationData[1], parentGlobalRotationData[2]);
                    const auto inverseParentRotation = parentRotation.inverse();

                    translation -= parentTranslation;
                    translation = inverseParentRotation * translation;
                    rotation = inverseParentRotation * rotation;
                }
                rotation.normalize();

                boneNode.name = bone.name;

                boneNode.translation = std::to_array({translation.x(), translation.y(), translation.z()});
                boneNode.rotation = std::to_array({rotation.x(), rotation.y(), rotation.z(), rotation.w()});

                std::vector<unsigned> children;
                for (auto maybeChildIndex = 0u; maybeChildIndex < boneCount; maybeChildIndex++)
                {
                    if (common.m_bones[maybeChildIndex].parentIndex && *common.m_bones[maybeChildIndex].parentIndex == boneIndex)
                        children.emplace_back(maybeChildIndex + m_first_bone_node);
                }
                if (!children.empty())
                    boneNode.children = std::move(children);

                gltf.nodes->emplace_back(std::move(boneNode));
            }
        }

        void CreateSkin(JsonRoot& gltf, const XModelCommon& xmodel) const
        {
            if (xmodel.m_bones.empty())
                return;

            if (!gltf.skins.has_value())
                gltf.skins.emplace();

            JsonSkin skin;

            const auto boneCount = xmodel.m_bones.size();

            skin.joints.reserve(boneCount);
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
                skin.joints.emplace_back(boneIndex + m_first_bone_node);

            if (!xmodel.m_bone_weight_data.weights.empty())
                skin.inverseBindMatrices = m_inverse_bind_matrices_accessor;

            skin.skeleton = m_first_bone_node;

            gltf.skins->emplace_back(std::move(skin));
        }

        void CreateScene(JsonRoot& gltf, const XModelCommon& xmodel) const
        {
            JsonScene scene;

            // Only add skin if the model has bones
            scene.nodes.emplace_back(m_root_node);

            if (!gltf.scenes.has_value())
                gltf.scenes.emplace();

            gltf.scenes->emplace_back(std::move(scene));
            gltf.scene = 0u;
        }

        void CreateBufferViews(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.bufferViews.has_value())
                gltf.bufferViews.emplace();

            unsigned bufferOffset = 0u;

            JsonBufferView vertexBufferView;
            vertexBufferView.buffer = 0u;
            vertexBufferView.byteOffset = bufferOffset;
            vertexBufferView.byteStride = static_cast<unsigned>(sizeof(GltfVertex));
            vertexBufferView.byteLength = static_cast<unsigned>(sizeof(GltfVertex) * xmodel.m_vertices.size());
            vertexBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
            bufferOffset += vertexBufferView.byteLength;

            m_vertex_buffer_view = static_cast<unsigned>(gltf.bufferViews->size());
            gltf.bufferViews->emplace_back(vertexBufferView);

            if (!xmodel.m_bone_weight_data.weights.empty())
            {
                JsonBufferView jointsBufferView;
                jointsBufferView.buffer = 0u;
                jointsBufferView.byteOffset = bufferOffset;
                jointsBufferView.byteLength = static_cast<unsigned>(sizeof(uint8_t) * xmodel.m_vertices.size() * 4u);
                jointsBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
                bufferOffset += jointsBufferView.byteLength;

                m_joints_buffer_view = static_cast<unsigned>(gltf.bufferViews->size());
                gltf.bufferViews->emplace_back(jointsBufferView);

                JsonBufferView weightsBufferView;
                weightsBufferView.buffer = 0u;
                weightsBufferView.byteOffset = bufferOffset;
                weightsBufferView.byteLength = static_cast<unsigned>(sizeof(float) * xmodel.m_vertices.size() * 4u);
                weightsBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
                bufferOffset += weightsBufferView.byteLength;

                m_weights_buffer_view = static_cast<unsigned>(gltf.bufferViews->size());
                gltf.bufferViews->emplace_back(weightsBufferView);

                JsonBufferView inverseBindMatricesBufferView;
                inverseBindMatricesBufferView.buffer = 0u;
                inverseBindMatricesBufferView.byteOffset = bufferOffset;
                inverseBindMatricesBufferView.byteLength = static_cast<unsigned>(sizeof(float) * xmodel.m_bones.size() * 16u);
                bufferOffset += inverseBindMatricesBufferView.byteLength;

                m_inverse_bind_matrices_buffer_view = static_cast<unsigned>(gltf.bufferViews->size());
                gltf.bufferViews->emplace_back(inverseBindMatricesBufferView);
            }

            m_first_index_buffer_view = static_cast<unsigned>(gltf.bufferViews->size());
            for (const auto& object : xmodel.m_objects)
            {
                JsonBufferView indicesBufferView;
                indicesBufferView.buffer = 0u;
                indicesBufferView.byteOffset = bufferOffset;
                indicesBufferView.byteLength = static_cast<unsigned>(sizeof(unsigned short) * object.m_faces.size() * 3u);
                indicesBufferView.target = JsonBufferViewTarget::ELEMENT_ARRAY_BUFFER;
                bufferOffset += indicesBufferView.byteLength;

                gltf.bufferViews->emplace_back(indicesBufferView);
            }
        }

        void CreateAccessors(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.accessors.has_value())
                gltf.accessors.emplace();

            JsonAccessor positionAccessor;
            positionAccessor.bufferView = m_vertex_buffer_view;
            positionAccessor.byteOffset = static_cast<unsigned>(offsetof(GltfVertex, coordinates));
            positionAccessor.componentType = JsonAccessorComponentType::FLOAT;
            positionAccessor.count = static_cast<unsigned>(xmodel.m_vertices.size());
            positionAccessor.type = JsonAccessorType::VEC3;
            m_position_accessor = static_cast<unsigned>(gltf.accessors->size());
            gltf.accessors->emplace_back(positionAccessor);

            JsonAccessor normalAccessor;
            normalAccessor.bufferView = m_vertex_buffer_view;
            normalAccessor.byteOffset = static_cast<unsigned>(offsetof(GltfVertex, normal));
            normalAccessor.componentType = JsonAccessorComponentType::FLOAT;
            normalAccessor.count = static_cast<unsigned>(xmodel.m_vertices.size());
            normalAccessor.type = JsonAccessorType::VEC3;
            m_normal_accessor = static_cast<unsigned>(gltf.accessors->size());
            gltf.accessors->emplace_back(normalAccessor);

            JsonAccessor uvAccessor;
            uvAccessor.bufferView = m_vertex_buffer_view;
            uvAccessor.byteOffset = static_cast<unsigned>(offsetof(GltfVertex, uv));
            uvAccessor.componentType = JsonAccessorComponentType::FLOAT;
            uvAccessor.count = static_cast<unsigned>(xmodel.m_vertices.size());
            uvAccessor.type = JsonAccessorType::VEC2;
            m_uv_accessor = static_cast<unsigned>(gltf.accessors->size());
            gltf.accessors->emplace_back(uvAccessor);

            if (!xmodel.m_bone_weight_data.weights.empty())
            {
                JsonAccessor jointsAccessor;
                jointsAccessor.bufferView = m_joints_buffer_view;
                jointsAccessor.componentType = JsonAccessorComponentType::UNSIGNED_BYTE;
                jointsAccessor.count = static_cast<unsigned>(xmodel.m_vertices.size());
                jointsAccessor.type = JsonAccessorType::VEC4;
                m_joints_accessor = static_cast<unsigned>(gltf.accessors->size());
                gltf.accessors->emplace_back(jointsAccessor);

                JsonAccessor weightsAccessor;
                weightsAccessor.bufferView = m_weights_buffer_view;
                weightsAccessor.componentType = JsonAccessorComponentType::FLOAT;
                weightsAccessor.count = static_cast<unsigned>(xmodel.m_vertices.size());
                weightsAccessor.type = JsonAccessorType::VEC4;
                m_weights_accessor = static_cast<unsigned>(gltf.accessors->size());
                gltf.accessors->emplace_back(weightsAccessor);

                JsonAccessor inverseBindMatricesAccessor;
                inverseBindMatricesAccessor.bufferView = m_inverse_bind_matrices_buffer_view;
                inverseBindMatricesAccessor.componentType = JsonAccessorComponentType::FLOAT;
                inverseBindMatricesAccessor.count = static_cast<unsigned>(xmodel.m_bones.size());
                inverseBindMatricesAccessor.type = JsonAccessorType::MAT4;
                m_inverse_bind_matrices_accessor = static_cast<unsigned>(gltf.accessors->size());
                gltf.accessors->emplace_back(inverseBindMatricesAccessor);
            }

            m_first_index_accessor = static_cast<unsigned>(gltf.accessors->size());
            for (auto i = 0u; i < xmodel.m_objects.size(); i++)
            {
                const auto& object = xmodel.m_objects[i];

                JsonAccessor indicesAccessor;
                indicesAccessor.bufferView = m_first_index_buffer_view + i;
                indicesAccessor.componentType = JsonAccessorComponentType::UNSIGNED_SHORT;
                indicesAccessor.count = static_cast<unsigned>(object.m_faces.size() * 3u);
                indicesAccessor.type = JsonAccessorType::SCALAR;

                gltf.accessors->emplace_back(indicesAccessor);
            }
        }

        void FillBufferData(JsonRoot& gltf, const XModelCommon& xmodel, std::vector<uint8_t>& bufferData) const
        {
            const auto expectedBufferSize = GetExpectedBufferSize(xmodel);
            bufferData.resize(expectedBufferSize);

            auto currentBufferOffset = 0uz;

            float minPosition[3]{
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
            };
            float maxPosition[3]{
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),
            };

            for (const auto& commonVertex : xmodel.m_vertices)
            {
                auto* vertex = reinterpret_cast<GltfVertex*>(&bufferData[currentBufferOffset]);

                vertex->coordinates[0] = commonVertex.coordinates[0];
                vertex->coordinates[1] = commonVertex.coordinates[1];
                vertex->coordinates[2] = commonVertex.coordinates[2];
                LhcToRhcCoordinates(vertex->coordinates);

                minPosition[0] = std::min(minPosition[0], vertex->coordinates[0]);
                minPosition[1] = std::min(minPosition[1], vertex->coordinates[1]);
                minPosition[2] = std::min(minPosition[2], vertex->coordinates[2]);
                maxPosition[0] = std::max(maxPosition[0], vertex->coordinates[0]);
                maxPosition[1] = std::max(maxPosition[1], vertex->coordinates[1]);
                maxPosition[2] = std::max(maxPosition[2], vertex->coordinates[2]);

                vertex->normal[0] = commonVertex.normal[0];
                vertex->normal[1] = commonVertex.normal[1];
                vertex->normal[2] = commonVertex.normal[2];
                LhcToRhcCoordinates(vertex->normal);

                vertex->uv[0] = commonVertex.uv[0];
                vertex->uv[1] = commonVertex.uv[1];

                currentBufferOffset += sizeof(GltfVertex);
            }

            if (gltf.accessors.has_value())
            {
                gltf.accessors.value()[m_position_accessor].min = std::vector({minPosition[0], minPosition[1], minPosition[2]});
                gltf.accessors.value()[m_position_accessor].max = std::vector({maxPosition[0], maxPosition[1], maxPosition[2]});
            }

            if (!xmodel.m_bone_weight_data.weights.empty())
            {
                assert(xmodel.m_vertex_bone_weights.size() == xmodel.m_vertices.size());

                auto* joints = &bufferData[currentBufferOffset];
                auto* weights = reinterpret_cast<float*>(&bufferData[currentBufferOffset + sizeof(uint8_t) * 4u * xmodel.m_vertex_bone_weights.size()]);
                for (const auto& commonVertexWeights : xmodel.m_vertex_bone_weights)
                {
                    assert(commonVertexWeights.weightOffset < xmodel.m_bone_weight_data.weights.size());
                    assert(commonVertexWeights.weightCount <= 4u);

                    const auto commonVertexWeightCount = std::min(commonVertexWeights.weightCount, 4u);
                    const auto* commonVertexWeightData = &xmodel.m_bone_weight_data.weights[commonVertexWeights.weightOffset];
                    for (auto i = 0u; i < commonVertexWeightCount; i++)
                    {
                        joints[i] = static_cast<unsigned char>(commonVertexWeightData[i].boneIndex);
                        weights[i] = commonVertexWeightData[i].weight;
                    }

                    for (auto i = commonVertexWeightCount; i < 4u; i++)
                    {
                        joints[i] = 0u;
                        weights[i] = 0.0f;
                    }

                    joints += 4u;
                    weights += 4u;
                }

                currentBufferOffset += (sizeof(uint8_t) + sizeof(float)) * 4u * xmodel.m_vertex_bone_weights.size();

                auto* inverseBindMatrixData = reinterpret_cast<float*>(&bufferData[currentBufferOffset]);
                for (const auto& bone : xmodel.m_bones)
                {
                    const auto translation = Eigen::Translation3f(bone.globalOffset[0], bone.globalOffset[1], bone.globalOffset[2]);
                    const auto rotation = Eigen::Quaternionf(bone.globalRotation.w, bone.globalRotation.x, bone.globalRotation.y, bone.globalRotation.z);
                    const auto bindMatrixTransform = translation * rotation;
                    auto bindMatrix = bindMatrixTransform.matrix();

                    LhcToRhcMatrix(bindMatrix);
                    const auto inverseBindMatrix = bindMatrix.inverse();

                    // GLTF matrix is column major
                    inverseBindMatrixData[0] = inverseBindMatrix(0, 0);
                    inverseBindMatrixData[1] = inverseBindMatrix(1, 0);
                    inverseBindMatrixData[2] = inverseBindMatrix(2, 0);
                    inverseBindMatrixData[3] = inverseBindMatrix(3, 0);
                    inverseBindMatrixData[4] = inverseBindMatrix(0, 1);
                    inverseBindMatrixData[5] = inverseBindMatrix(1, 1);
                    inverseBindMatrixData[6] = inverseBindMatrix(2, 1);
                    inverseBindMatrixData[7] = inverseBindMatrix(3, 1);
                    inverseBindMatrixData[8] = inverseBindMatrix(0, 2);
                    inverseBindMatrixData[9] = inverseBindMatrix(1, 2);
                    inverseBindMatrixData[10] = inverseBindMatrix(2, 2);
                    inverseBindMatrixData[11] = inverseBindMatrix(3, 2);
                    inverseBindMatrixData[12] = inverseBindMatrix(0, 3);
                    inverseBindMatrixData[13] = inverseBindMatrix(1, 3);
                    inverseBindMatrixData[14] = inverseBindMatrix(2, 3);
                    inverseBindMatrixData[15] = inverseBindMatrix(3, 3);

                    inverseBindMatrixData += 16u;
                }
                currentBufferOffset += sizeof(float) * 16u * xmodel.m_bones.size();
            }

            for (const auto& object : xmodel.m_objects)
            {
                for (const auto& face : object.m_faces)
                {
                    auto* faceIndices = reinterpret_cast<unsigned short*>(&bufferData[currentBufferOffset]);
                    faceIndices[0] = static_cast<unsigned short>(face.vertexIndex[0]);
                    faceIndices[1] = static_cast<unsigned short>(face.vertexIndex[1]);
                    faceIndices[2] = static_cast<unsigned short>(face.vertexIndex[2]);
                    LhcToRhcIndices(faceIndices);

                    currentBufferOffset += sizeof(unsigned short) * 3u;
                }
            }

            assert(expectedBufferSize == currentBufferOffset);
        }

        static size_t GetExpectedBufferSize(const XModelCommon& xmodel)
        {
            auto result = 0uz;

            result += xmodel.m_vertices.size() * sizeof(GltfVertex);

            if (!xmodel.m_bone_weight_data.weights.empty())
            {
                // Joints and weights
                result += xmodel.m_vertices.size() * 4u * (sizeof(uint8_t) + sizeof(float));

                // Inverse bind matrices
                result += xmodel.m_bones.size() * 16u * sizeof(float);
            }

            for (const auto& object : xmodel.m_objects)
            {
                result += object.m_faces.size() * sizeof(unsigned short) * 3u;
            }

            return result;
        }

        void CreateBuffer(JsonRoot& gltf, const XModelCommon& xmodel, const std::vector<uint8_t>& bufferData) const
        {
            if (!gltf.buffers.has_value())
                gltf.buffers.emplace();

            JsonBuffer jsonBuffer;
            jsonBuffer.byteLength = static_cast<unsigned>(bufferData.size());

            if (!bufferData.empty())
                jsonBuffer.uri = m_output->CreateBufferUri(bufferData.data(), bufferData.size());

            gltf.buffers->emplace_back(std::move(jsonBuffer));
        }

        unsigned m_first_mesh_node = 0u;
        unsigned m_root_node = 0u;
        unsigned m_first_bone_node = 0u;
        unsigned m_position_accessor = 0u;
        unsigned m_normal_accessor = 0u;
        unsigned m_uv_accessor = 0u;
        unsigned m_joints_accessor = 0u;
        unsigned m_weights_accessor = 0u;
        unsigned m_inverse_bind_matrices_accessor = 0u;
        unsigned m_vertex_buffer_view = 0u;
        unsigned m_joints_buffer_view = 0u;
        unsigned m_weights_buffer_view = 0u;
        unsigned m_inverse_bind_matrices_buffer_view = 0u;
        unsigned m_first_index_buffer_view = 0u;
        unsigned m_first_index_accessor = 0u;

        const Output* m_output;
        std::string m_game_name;
        std::string m_zone_name;
    };
} // namespace

std::unique_ptr<Writer> Writer::CreateWriter(const Output* output, std::string gameName, std::string zoneName)
{
    return std::make_unique<GltfWriterImpl>(output, std::move(gameName), std::move(zoneName));
}
