#include "XModel/Gltf/GltfWriter.h"

#include "XModel/Gltf/GltfOutput.h"
#include "XModel/XModelCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using namespace gltf;

namespace
{
    class MockGltfOutput final : public Output
    {
    public:
        std::optional<std::string> CreateBufferUri(const void*, size_t) const override
        {
            return "buffer.bin";
        }

        void EmitJson(const nlohmann::ordered_json& json) const override
        {
            m_json = json;
        }

        void EmitBuffer(const void* buffer, const size_t bufferSize) const override
        {
            const auto* bytes = static_cast<const uint8_t*>(buffer);
            m_buffer.assign(bytes, bytes + bufferSize);
        }

        void Finalize() const override {}

        mutable nlohmann::ordered_json m_json;
        mutable std::vector<uint8_t> m_buffer;
    };

    XModelCommon GivenModelWithVertexCount(const unsigned vertexCount, const unsigned highIndex)
    {
        XModelCommon xmodel;
        xmodel.m_name = "big_model";

        XModelMaterial material;
        material.ApplyDefaults();
        material.name = "mtl";
        xmodel.m_materials.emplace_back(std::move(material));

        xmodel.m_vertices.resize(vertexCount, XModelVertex{});

        XModelObject object;
        object.name = "obj";
        object.materialIndex = 0u;
        object.m_faces.push_back(XModelFace{{0u, 1u, highIndex}});
        xmodel.m_objects.emplace_back(std::move(object));

        return xmodel;
    }

    const nlohmann::ordered_json* FindIndexAccessor(const nlohmann::ordered_json& gltf)
    {
        for (const auto& accessor : gltf.at("accessors"))
        {
            if (accessor.at("type") == "SCALAR")
                return &accessor;
        }
        return nullptr;
    }
} // namespace

TEST_CASE("GltfWriter: Uses 32-bit indices for models with more than 65535 vertices", "[gltf][xmodel][objwriter]")
{
    // A single vertex buffer is shared by every primitive, so an index >= 65536 cannot be encoded as
    // UNSIGNED_SHORT. Stock behaviour truncated it modulo 65536, silently corrupting the mesh.
    constexpr auto vertexCount = 70000u;
    constexpr auto highIndex = 69999u;

    const auto xmodel = GivenModelWithVertexCount(vertexCount, highIndex);

    MockGltfOutput output;
    const auto writer = Writer::CreateWriter(&output, "IW4", "test");
    writer->Write(xmodel);

    const auto* indexAccessor = FindIndexAccessor(output.m_json);
    REQUIRE(indexAccessor != nullptr);

    // 5125 == UNSIGNED_INT. Before the fix this was a hardcoded 5123 (UNSIGNED_SHORT).
    REQUIRE(indexAccessor->at("componentType").get<unsigned>() == 5125u);

    const auto bufferViewIndex = indexAccessor->at("bufferView").get<size_t>();
    const auto byteOffset = output.m_json.at("bufferViews").at(bufferViewIndex).at("byteOffset").get<size_t>();

    REQUIRE(output.m_buffer.size() >= byteOffset + sizeof(unsigned int) * 3u);

    unsigned int writtenIndices[3];
    std::memcpy(writtenIndices, &output.m_buffer[byteOffset], sizeof(writtenIndices));

    // Winding is reversed for the LHC -> RHC conversion, so face {0, 1, highIndex} is stored as
    // {highIndex, 1, 0}. The high index must survive intact rather than wrapping to highIndex % 65536.
    REQUIRE(writtenIndices[0] == highIndex);
    REQUIRE(writtenIndices[1] == 1u);
    REQUIRE(writtenIndices[2] == 0u);
}
