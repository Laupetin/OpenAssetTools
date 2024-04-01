#include "GltfWriter.h"

#include "GitVersion.h"
#include "XModel/Gltf/GltfConstants.h"
#include "XModel/Gltf/JsonGltf.h"

using namespace gltf;
using namespace nlohmann;

namespace
{
    constexpr auto GLTF_GENERATOR = "OpenAssetTools " GIT_VERSION;

    class GltfWriterImpl final : public gltf::Writer
    {
        static constexpr auto NODE_INDEX_MESH = 0u;
        static constexpr auto NODE_FIRST_INDEX_BONES = 1u;

    public:
        GltfWriterImpl(const Output* output, std::string gameName, std::string zoneName)
            : m_output(output),
              m_game_name(std::move(gameName)),
              m_zone_name(std::move(zoneName))
        {
        }

        void Write(std::ostream& stream) override
        {
            JsonRoot gltf;
            CreateJsonAsset(gltf.asset);
            CreateMeshNode(gltf);
            CreateMesh(gltf);
            CreateSkeletonNodes(gltf);
            CreateSkin(gltf);
            CreateScene(gltf);

            const json jRoot = gltf;
            m_output->EmitJson(jRoot);
            m_output->Finalize();
        }

    private:
        static void CreateJsonAsset(JsonAsset& asset)
        {
            asset.version = GLTF_VERSION_STRING;
            asset.generator = GLTF_GENERATOR;
        }

        void CreateMeshNode(JsonRoot& gltf) const
        {
            JsonNode meshNode;

            // We only have one mesh
            meshNode.mesh = 0u;

            // Only add skin if the model has bones
            if (!m_bones.empty())
            {
                // We only have one skin
                meshNode.skin = 0u;
            }

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            gltf.nodes->emplace_back(std::move(meshNode));
        }

        void CreateMesh(JsonRoot& gltf) const
        {
            if (!gltf.meshes.has_value())
                gltf.meshes.emplace();

            JsonMesh mesh;

            gltf.meshes->emplace_back(std::move(mesh));
        }

        void CreateSkeletonNodes(JsonRoot& gltf) const
        {
            if (m_bones.empty())
                return;

            if (!gltf.nodes.has_value())
                gltf.nodes.emplace();

            const auto boneCount = m_bones.size();
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
            {
                JsonNode boneNode;
                const auto& bone = m_bones[boneIndex];

                boneNode.name = bone.name;
                boneNode.translation = std::to_array(bone.globalOffset);
                boneNode.rotation = std::to_array({bone.globalRotation.m_x, bone.globalRotation.m_y, bone.globalRotation.m_z, bone.globalRotation.m_w});

                std::vector<unsigned> children;
                for (auto maybeChildIndex = 0u; maybeChildIndex < boneCount; maybeChildIndex++)
                {
                    if (m_bones[maybeChildIndex].parentIndex == static_cast<int>(boneIndex))
                        children.emplace_back(boneIndex + NODE_FIRST_INDEX_BONES);
                }
                if (!children.empty())
                    boneNode.children = std::move(children);

                gltf.nodes->emplace_back(std::move(boneNode));
            }
        }

        void CreateSkin(JsonRoot& gltf) const
        {
            if (m_bones.empty())
                return;

            if (!gltf.skins.has_value())
                gltf.skins.emplace();

            JsonSkin skin;

            const auto boneCount = m_bones.size();

            skin.joints.reserve(boneCount);
            for (auto boneIndex = 0u; boneIndex < boneCount; boneIndex++)
                skin.joints.emplace_back(boneIndex + NODE_FIRST_INDEX_BONES);

            gltf.skins->emplace_back(std::move(skin));
        }

        void CreateScene(JsonRoot& gltf) const
        {
            JsonScene scene;

            // Only add skin if the model has bones
            if (m_bones.empty())
                scene.nodes.emplace_back(NODE_INDEX_MESH);
            else
                scene.nodes.emplace_back(m_skeleton_node);

            if (!gltf.scenes.has_value())
                gltf.scenes.emplace();

            gltf.scenes->emplace_back(std::move(scene));
            gltf.scene = 0u;
        }

        unsigned m_skeleton_node = 0u;

        const Output* m_output;
        std::string m_game_name;
        std::string m_zone_name;
    };
} // namespace

std::unique_ptr<Writer> Writer::CreateWriter(const Output* output, std::string gameName, std::string zoneName)
{
    return std::make_unique<GltfWriterImpl>(output, std::move(gameName), std::move(zoneName));
}
