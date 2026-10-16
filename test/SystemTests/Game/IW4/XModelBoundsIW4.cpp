#include "Game/IW4/IW4.h"
#include "Gdt/GdtLookup.h"
#include "IObjLoader.h"
#include "SearchPath/MockSearchPath.h"
#include "XModel/Gltf/GltfBinOutput.h"
#include "XModel/Gltf/GltfWriter.h"
#include "XModel/XModelCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <nlohmann/json.hpp>
#include <sstream>

namespace
{
    TEST_CASE("IW4 model bounds enclose geometry away from the origin", "[iw4][system][xmodel][bounds]")
    {
        const auto offset = GENERATE(10.0f, -30.0f);
        CAPTURE(offset);
        XModelCommon common;
        common.m_name = "triangle";
        common.m_vertices = {
            {.coordinates = {offset, offset, offset},     .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {0, 0}},
            {.coordinates = {offset + 2, offset, offset}, .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {1, 0}},
            {.coordinates = {offset, offset + 4, offset}, .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {0, 1}},
        };
        XModelMaterial material{};
        material.ApplyDefaults();
        material.name = "material";
        common.m_materials.push_back(material);
        common.m_objects.push_back({.name = "triangle", .materialIndex = 0, .m_faces = {XModelFace{{0, 1, 2}}}});
        std::ostringstream modelData(std::ios::binary);
        const gltf::BinOutput modelOutput(modelData);
        gltf::Writer::CreateWriter(&modelOutput, "iw4", "test")->Write(common);
        const nlohmann::json metadata{
            {"_game",    "iw4"                                          },
            {"_type",    "xmodel"                                       },
            {"_version", 2                                              },
            {"flags",    0                                              },
            {"lods",     {{{"file", "triangle.glb"}, {"distance", 100}}}},
        };
        MockSearchPath searchPath;
        searchPath.AddFileData("triangle.glb", modelData.str());
        searchPath.AddFileData("xmodel/triangle.json", metadata.dump());
        searchPath.AddFileData("partclassification.csv", "");
        Zone zone("bounds", 0, GameId::IW4, GamePlatform::PC);
        AssetCreatorCollection creators(zone);
        GdtLookup gdt;
        IObjLoader::GetObjLoaderForGame(GameId::IW4)->ConfigureCreatorCollection(creators, zone, searchPath, gdt);
        IgnoredAssetLookup ignored;
        ignored.m_ignored_asset_lookup.emplace("material", IW4::ASSET_TYPE_MATERIAL);
        AssetCreationContext context(zone, &creators, &ignored);
        REQUIRE(creators.CreateAsset(IW4::ASSET_TYPE_XMODEL, "triangle", context).HasBeenSuccessful());
        const auto* asset = zone.m_pools.GetAsset<IW4::AssetXModel>("triangle");
        REQUIRE(asset);
        const auto& model = *asset->Asset();
        CHECK_THAT(model.bounds.midPoint.x, Catch::Matchers::WithinAbs(offset + 1.0f, 0.0001f));
        CHECK_THAT(model.bounds.midPoint.y, Catch::Matchers::WithinAbs(offset + 2.0f, 0.0001f));
        CHECK_THAT(model.bounds.midPoint.z, Catch::Matchers::WithinAbs(offset, 0.0001f));
        CHECK_THAT(model.bounds.halfSize.x, Catch::Matchers::WithinAbs(1.0f, 0.0001f));
        CHECK_THAT(model.bounds.halfSize.y, Catch::Matchers::WithinAbs(2.0f, 0.0001f));
        CHECK(model.bounds.halfSize.z == 0.0f);
        CHECK_THAT(model.radius, Catch::Matchers::WithinAbs(std::sqrt(5.0f), 0.0001f));
    }
} // namespace
