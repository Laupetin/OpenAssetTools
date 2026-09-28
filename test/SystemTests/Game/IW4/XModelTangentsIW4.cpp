#include "Game/IW4/CommonIW4.h"
#include "Gdt/GdtLookup.h"
#include "IObjLoader.h"
#include "SearchPath/MockSearchPath.h"
#include "XModel/Gltf/GltfBinOutput.h"
#include "XModel/Gltf/GltfWriter.h"
#include "XModel/XModelCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

namespace
{
    TEST_CASE("IW4 model loading preserves tangent axes and mirrored UV handedness", "[iw4][system][xmodel]")
    {
        const auto mirrored = GENERATE(false, true);
        XModelCommon common;
        common.m_name = "triangle";
        common.m_vertices = {
            {.coordinates = {0, 0, 0}, .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {0, 0}                      },
            {.coordinates = {1, 0, 0}, .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {1, 0}                      },
            {.coordinates = {0, 1, 0}, .normal = {0, 0, 1}, .color = {1, 1, 1, 1}, .uv = {0, mirrored ? -1.0f : 1.0f}},
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
        Zone zone("tangents", 0, GameVariantId::IW4_PC32);
        AssetCreatorCollection creators(zone);
        GdtLookup gdt;
        IObjLoader::GetObjLoaderForGame(GameId::IW4)->ConfigureCreatorCollection(creators, zone, searchPath, gdt);
        IgnoredAssetLookup ignored;
        ignored.m_ignored_asset_lookup.emplace("material", IW4::ASSET_TYPE_MATERIAL);
        AssetCreationContext context(zone, &creators, &ignored);
        REQUIRE(creators.CreateAsset(IW4::ASSET_TYPE_XMODEL, "triangle", context).HasBeenSuccessful());
        for (auto i = 0u; i < 3u; ++i)
        {
            float tangent[3];
            const auto& vertex = zone.m_pools.GetAsset<IW4::AssetXModel>("triangle")->Asset()->lodInfo[0].modelSurfs->surfs[0].verts0[i];
            IW4::Common::Vec3UnpackUnitVec(vertex.tangent, tangent);
            CHECK_THAT(tangent[0], Catch::Matchers::WithinAbs(1.0f, 0.01f));
            CHECK_THAT(tangent[1], Catch::Matchers::WithinAbs(0.0f, 0.01f));
            CHECK_THAT(tangent[2], Catch::Matchers::WithinAbs(0.0f, 0.01f));
            CHECK(vertex.binormalSign == (mirrored ? -1.0f : 1.0f));
        }
    }
} // namespace
