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
            gltf.asset.version = GLTF_VERSION_STRING;
            gltf.asset.generator = GLTF_GENERATOR;

            const json jRoot = gltf;
            m_output->EmitJson(jRoot);
            m_output->Finalize();
        }

    private:
        const Output* m_output;
        std::string m_game_name;
        std::string m_zone_name;
    };
} // namespace

std::unique_ptr<Writer> Writer::CreateWriter(const Output* output, std::string gameName, std::string zoneName)
{
    return std::make_unique<GltfWriterImpl>(output, std::move(gameName), std::move(zoneName));
}
