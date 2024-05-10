#include "GltfWriter.h"

#include "GitVersion.h"
#include "Math/Vector.h"
#include "XModel/Gltf/GltfConstants.h"
#include "XModel/Gltf/JsonGltf.h"

#include <format>

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
            CreateMeshNode(gltf, xmodel);
            CreateSkeletonNodes(gltf, xmodel);
            CreateMaterials(gltf, xmodel);
            CreateSkin(gltf, xmodel);
            CreateBufferViews(gltf, xmodel);
            CreateAccessors(gltf, xmodel);
            CreateMesh(gltf, xmodel);
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

        void CreateMeshNode(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            JsonNode meshNode;

            // We only have one mesh
            meshNode.mesh = 0u;

            // Only add skin if the model has bones
            if (!xmodel.m_bones.empty())
            {
                // We only have one skin
                meshNode.skin = 0u;
            }

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            m_mesh_node = gltf.nodes->size();
            gltf.nodes->emplace_back(std::move(meshNode));
        }

        void CreateMesh(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (!gltf.meshes.has_value())
                gltf.meshes.emplace();

            JsonMesh mesh;

            const auto hasBoneWeightData = !xmodel.m_vertex_bone_weights.empty();

            auto objectIndex = 0u;
            for (const auto& object : xmodel.m_objects)
            {
                JsonMeshPrimitives primitives;

                if (object.materialIndex >= 0)
                    primitives.material = static_cast<unsigned>(object.materialIndex);

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
            }

            gltf.meshes->emplace_back(std::move(mesh));
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
            const auto imageIndex = gltf.images->size();
            gltf.images->emplace_back(std::move(image));

            JsonTexture texture;
            texture.source = imageIndex;
            const auto textureIndex = gltf.textures->size();
            gltf.textures->emplace_back(texture);

            return textureIndex;
        }

        void CreateSkeletonNodes(JsonRoot& gltf, const XModelCommon& xmodel)
        {
            if (xmodel.m_bones.empty())
                return;

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            const auto boneCount = xmodel.m_bones.size();
            m_first_bone_node = gltf.nodes->size();
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
            {
                JsonNode boneNode;
                const auto& bone = xmodel.m_bones[boneIndex];

                Vector3f translation(bone.globalOffset[0], bone.globalOffset[2], -bone.globalOffset[1]);
                Quaternion32 rotation(bone.globalRotation.m_x, bone.globalRotation.m_z, -bone.globalRotation.m_y, bone.globalRotation.m_w);
                if (bone.parentIndex >= 0)
                {
                    const auto& parentBone = xmodel.m_bones[bone.parentIndex];
                    translation -= Vector3f(parentBone.globalOffset[0], parentBone.globalOffset[2], -parentBone.globalOffset[1]);
                    rotation /= Quaternion32(
                        parentBone.globalRotation.m_x, parentBone.globalRotation.m_z, -parentBone.globalRotation.m_y, parentBone.globalRotation.m_w);
                }
                rotation.Normalize();

                boneNode.name = bone.name;
                boneNode.translation = std::to_array({translation.x(), translation.y(), translation.z()});
                boneNode.rotation = std::to_array({rotation.m_x, rotation.m_y, rotation.m_z, rotation.m_w});

                std::vector<unsigned> children;
                for (auto maybeChildIndex = 0u; maybeChildIndex < boneCount; maybeChildIndex++)
                {
                    if (xmodel.m_bones[maybeChildIndex].parentIndex == static_cast<int>(boneIndex))
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

            gltf.skins->emplace_back(std::move(skin));
        }

        void CreateScene(JsonRoot& gltf, const XModelCommon& xmodel) const
        {
            JsonScene scene;

            // Only add skin if the model has bones
            scene.nodes.emplace_back(m_mesh_node);

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
            vertexBufferView.byteStride = sizeof(GltfVertex);
            vertexBufferView.byteLength = sizeof(GltfVertex) * xmodel.m_vertices.size();
            vertexBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
            bufferOffset += vertexBufferView.byteLength;

            m_vertex_buffer_view = gltf.bufferViews->size();
            gltf.bufferViews->emplace_back(vertexBufferView);

            if (!xmodel.m_vertex_bone_weights.empty())
            {
                JsonBufferView jointsBufferView;
                jointsBufferView.buffer = 0u;
                jointsBufferView.byteOffset = bufferOffset;
                jointsBufferView.byteLength = sizeof(uint8_t) * xmodel.m_vertices.size() * 4u;
                jointsBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
                bufferOffset += jointsBufferView.byteLength;

                m_joints_buffer_view = gltf.bufferViews->size();
                gltf.bufferViews->emplace_back(jointsBufferView);

                JsonBufferView weightsBufferView;
                weightsBufferView.buffer = 0u;
                weightsBufferView.byteOffset = bufferOffset;
                weightsBufferView.byteLength = sizeof(float) * xmodel.m_vertices.size() * 4u;
                weightsBufferView.target = JsonBufferViewTarget::ARRAY_BUFFER;
                bufferOffset += weightsBufferView.byteLength;

                m_weights_buffer_view = gltf.bufferViews->size();
                gltf.bufferViews->emplace_back(weightsBufferView);
            }

            m_first_index_buffer_view = gltf.bufferViews->size();
            for (const auto& object : xmodel.m_objects)
            {
                JsonBufferView indicesBufferView;
                indicesBufferView.buffer = 0u;
                indicesBufferView.byteOffset = bufferOffset;
                indicesBufferView.byteLength = sizeof(unsigned short) * object.m_faces.size() * 3u;
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
            positionAccessor.byteOffset = offsetof(GltfVertex, coordinates);
            positionAccessor.componentType = JsonAccessorComponentType::FLOAT;
            positionAccessor.count = xmodel.m_vertices.size();
            positionAccessor.type = JsonAccessorType::VEC3;
            m_position_accessor = gltf.accessors->size();
            gltf.accessors->emplace_back(positionAccessor);

            JsonAccessor normalAccessor;
            normalAccessor.bufferView = m_vertex_buffer_view;
            normalAccessor.byteOffset = offsetof(GltfVertex, normal);
            normalAccessor.componentType = JsonAccessorComponentType::FLOAT;
            normalAccessor.count = xmodel.m_vertices.size();
            normalAccessor.type = JsonAccessorType::VEC3;
            m_normal_accessor = gltf.accessors->size();
            gltf.accessors->emplace_back(normalAccessor);

            JsonAccessor uvAccessor;
            uvAccessor.bufferView = m_vertex_buffer_view;
            uvAccessor.byteOffset = offsetof(GltfVertex, uv);
            uvAccessor.componentType = JsonAccessorComponentType::FLOAT;
            uvAccessor.count = xmodel.m_vertices.size();
            uvAccessor.type = JsonAccessorType::VEC2;
            m_uv_accessor = gltf.accessors->size();
            gltf.accessors->emplace_back(uvAccessor);

            if (!xmodel.m_vertex_bone_weights.empty())
            {
                JsonAccessor jointsAccessor;
                jointsAccessor.bufferView = m_joints_buffer_view;
                jointsAccessor.componentType = JsonAccessorComponentType::UNSIGNED_BYTE;
                jointsAccessor.count = xmodel.m_vertices.size();
                jointsAccessor.type = JsonAccessorType::VEC4;
                m_joints_accessor = gltf.accessors->size();
                gltf.accessors->emplace_back(jointsAccessor);

                JsonAccessor weightsAccessor;
                weightsAccessor.bufferView = m_weights_buffer_view;
                weightsAccessor.componentType = JsonAccessorComponentType::FLOAT;
                weightsAccessor.count = xmodel.m_vertices.size();
                weightsAccessor.type = JsonAccessorType::VEC4;
                m_weights_accessor = gltf.accessors->size();
                gltf.accessors->emplace_back(weightsAccessor);
            }

            m_first_index_accessor = gltf.accessors->size();
            for (auto i = 0u; i < xmodel.m_objects.size(); i++)
            {
                const auto& object = xmodel.m_objects[i];

                JsonAccessor indicesAccessor;
                indicesAccessor.bufferView = m_first_index_buffer_view + i;
                indicesAccessor.componentType = JsonAccessorComponentType::UNSIGNED_SHORT;
                indicesAccessor.count = object.m_faces.size() * 3u;
                indicesAccessor.type = JsonAccessorType::SCALAR;

                gltf.accessors->emplace_back(indicesAccessor);
            }
        }

        void FillBufferData(JsonRoot& gltf, const XModelCommon& xmodel, std::vector<uint8_t>& bufferData) const
        {
            const auto expectedBufferSize = GetExpectedBufferSize(xmodel);
            bufferData.resize(expectedBufferSize);

            auto currentBufferOffset = 0u;

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
                vertex->coordinates[1] = commonVertex.coordinates[2];
                vertex->coordinates[2] = -commonVertex.coordinates[1];

                if (minPosition[0] > vertex->coordinates[0])
                    minPosition[0] = vertex->coordinates[0];
                if (minPosition[1] > vertex->coordinates[1])
                    minPosition[1] = vertex->coordinates[1];
                if (minPosition[2] > vertex->coordinates[2])
                    minPosition[2] = vertex->coordinates[2];
                if (maxPosition[0] < vertex->coordinates[0])
                    maxPosition[0] = vertex->coordinates[0];
                if (maxPosition[1] < vertex->coordinates[1])
                    maxPosition[1] = vertex->coordinates[1];
                if (maxPosition[2] < vertex->coordinates[2])
                    maxPosition[2] = vertex->coordinates[2];

                vertex->normal[0] = commonVertex.normal[0];
                vertex->normal[1] = commonVertex.normal[2];
                vertex->normal[2] = -commonVertex.normal[1];

                vertex->uv[0] = commonVertex.uv[0];
                vertex->uv[1] = commonVertex.uv[1];

                currentBufferOffset += sizeof(GltfVertex);
            }

            if (gltf.accessors.has_value())
            {
                gltf.accessors.value()[m_position_accessor].min = std::vector({minPosition[0], minPosition[1], minPosition[2]});
                gltf.accessors.value()[m_position_accessor].max = std::vector({maxPosition[0], maxPosition[1], maxPosition[2]});
            }

            if (!xmodel.m_vertex_bone_weights.empty())
            {
                assert(xmodel.m_vertex_bone_weights.size() == xmodel.m_vertices.size());

                auto* joints = reinterpret_cast<uint8_t*>(&bufferData[currentBufferOffset]);
                auto* weights = reinterpret_cast<float*>(&bufferData[currentBufferOffset + sizeof(uint8_t) * 4u * xmodel.m_vertex_bone_weights.size()]);
                for (const auto& commonVertexWeights : xmodel.m_vertex_bone_weights)
                {
                    assert(commonVertexWeights.weights != nullptr);
                    assert(commonVertexWeights.weightCount <= 4u);

                    const auto commonVertexWeightCount = std::min(commonVertexWeights.weightCount, 4u);
                    for (auto i = 0u; i < commonVertexWeightCount; i++)
                    {
                        joints[i] = static_cast<unsigned char>(commonVertexWeights.weights[i].boneIndex);
                        weights[i] = commonVertexWeights.weights[i].weight;
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
            }

            for (const auto& object : xmodel.m_objects)
            {
                for (const auto& face : object.m_faces)
                {
                    auto* faceIndices = reinterpret_cast<unsigned short*>(&bufferData[currentBufferOffset]);
                    faceIndices[0] = static_cast<unsigned short>(face.vertexIndex[2]);
                    faceIndices[1] = static_cast<unsigned short>(face.vertexIndex[1]);
                    faceIndices[2] = static_cast<unsigned short>(face.vertexIndex[0]);

                    currentBufferOffset += sizeof(unsigned short) * 3u;
                }
            }

            assert(expectedBufferSize == currentBufferOffset);
        }

        static size_t GetExpectedBufferSize(const XModelCommon& xmodel)
        {
            auto result = 0u;

            result += xmodel.m_vertices.size() * sizeof(GltfVertex);

            if (!xmodel.m_vertex_bone_weights.empty())
            {
                result += xmodel.m_vertices.size() * 4u * (sizeof(uint8_t) + sizeof(float));
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
            jsonBuffer.byteLength = bufferData.size();

            if (!bufferData.empty())
                jsonBuffer.uri = m_output->CreateBufferUri(bufferData.data(), bufferData.size());

            gltf.buffers->emplace_back(std::move(jsonBuffer));
        }

        unsigned m_mesh_node = 0u;
        unsigned m_first_bone_node = 0u;
        unsigned m_position_accessor = 0u;
        unsigned m_normal_accessor = 0u;
        unsigned m_uv_accessor = 0u;
        unsigned m_joints_accessor = 0u;
        unsigned m_weights_accessor = 0u;
        unsigned m_vertex_buffer_view = 0u;
        unsigned m_joints_buffer_view = 0u;
        unsigned m_weights_buffer_view = 0u;
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