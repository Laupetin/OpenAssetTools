#include "Game/T4/XModel/XModelToCommonConverterT4.h"
#include "XModel/Gltf/GltfWriter.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using namespace T4;

namespace
{
    class MockGltfOutput final : public gltf::Output
    {
    public:
        std::optional<std::string> CreateBufferUri(const void* buffer, size_t bufferSize) const override
        {
            return std::nullopt;
        }

        void EmitJson(const nlohmann::ordered_json& json) const override
        {
            m_json = json;
        }

        void EmitBuffer(const void* buffer, size_t bufferSize) const override {}

        void Finalize() const override {}

        mutable nlohmann::ordered_json m_json;
    };

    TEST_CASE("XModelGltfWriterT4: Can write a non-contiguous root bone", "[t4][xmodel][gltf]")
    {
        constexpr std::array boneNameValues{
            "tag_weapon",
            "tag_brass",
            "tag_flash",
            "tag_sights",
            "tag_sights_front",
            "tag_clip",
        };

        Zone zone("MockZone", 0, GameId::T4, GamePlatform::PC);

        std::array<ScriptString, boneNameValues.size()> boneNames{};
        for (auto boneIndex = 0u; boneIndex < boneNameValues.size(); boneIndex++)
            boneNames[boneIndex] = zone.m_script_strings.AddOrGetScriptString(boneNameValues[boneIndex]);

        // The final offset is malformed in the source model: bone 5 minus 6 would be -1.
        // Treating the subtraction as unsigned used to turn this into UINT_MAX and crash the GLTF writer.
        std::array<unsigned char, boneNameValues.size() - 1> parentList{1, 2, 3, 4, 6};
        std::array<XModelQuat, boneNameValues.size() - 1> quats{};
        std::array<float, (boneNameValues.size() - 1) * 3> trans{};
        std::array<DObjAnimMat, boneNameValues.size()> baseMats{};
        for (auto& baseMat : baseMats)
            baseMat.quat.w = 1.0f;

        XModel model{};
        model.name = "bo2_worldmodel_an94";
        model.numBones = static_cast<unsigned char>(boneNames.size());
        model.numRootBones = 1;
        model.boneNames = boneNames.data();
        model.parentList = parentList.data();
        model.quats = quats.data();
        model.trans = trans.data();
        model.baseMat = baseMats.data();
        model.numLods = 1;

        XAssetInfo<XModel> assetInfo(ASSET_TYPE_XMODEL, model.name, &model);
        assetInfo.m_zone = &zone;

        xmodel::ToCommonConverterT4 converter;
        const auto common = converter.Convert(assetInfo, 0);

        REQUIRE(common.has_value());
        REQUIRE(common->m_bones.size() == boneNameValues.size());
        REQUIRE(common->m_bones[0].parentIndex == std::nullopt);
        REQUIRE(common->m_bones[1].parentIndex == 0u);
        REQUIRE(common->m_bones[2].parentIndex == 0u);
        REQUIRE(common->m_bones[3].parentIndex == 0u);
        REQUIRE(common->m_bones[4].parentIndex == 0u);
        REQUIRE(common->m_bones[5].parentIndex == std::nullopt);

        MockGltfOutput output;
        const auto writer = gltf::Writer::CreateWriter(&output, "T4", zone.m_name);
        writer->Write(*common);

        REQUIRE(output.m_json["nodes"].size() == 7);
        REQUIRE(output.m_json["nodes"][0]["children"] == nlohmann::ordered_json::array({1, 2, 3, 4}));
        REQUIRE(output.m_json["nodes"][6]["children"] == nlohmann::ordered_json::array({0, 5}));
        REQUIRE(output.m_json["skins"].size() == 1);
        REQUIRE_FALSE(output.m_json["skins"][0].contains("skeleton"));
    }
} // namespace
