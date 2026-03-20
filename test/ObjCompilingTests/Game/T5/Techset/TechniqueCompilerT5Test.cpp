#include "Game/T5/Techset/TechniqueCompilerT5.h"

#include "Game/T5/T5.h"
#include "Game/T5/Techset/PixelShaderLoaderT5.h"
#include "Game/T5/Techset/VertexDeclCompilerT5.h"
#include "Game/T5/Techset/VertexShaderLoaderT5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Shader/ShaderCommon.h"
#include "Utils/MemoryManager.h"
#include "catch2/generators/catch_generators.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>

using namespace T5;
using namespace Catch;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    void GivenVertexShaderFile(const std::string& name, MockSearchPath& searchPath)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/T5/Techset" / std::format("vs_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        std::string data(fileSize, '\0');
        file.read(data.data(), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        searchPath.AddFileData(shader::GetFileNameForVertexShaderAssetName(name), std::move(data));
    }

    void GivenPixelShaderFile(const std::string& name, MockSearchPath& searchPath)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/T5/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        std::string data(fileSize, '\0');
        file.read(data.data(), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        searchPath.AddFileData(shader::GetFileNameForPixelShaderAssetName(name), std::move(data));
    }
} // namespace

TEST_CASE("TechniqueCompilerT5", "[t5][techset][compiler]")
{

    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
    MockSearchPath searchPath;

    creatorCollection.AddSubAssetCreator(techset::CreateVertexDeclCompilerT5(memory));
    creatorCollection.AddSubAssetCreator(techset::CreateVertexShaderLoaderT5(memory, searchPath));
    creatorCollection.AddSubAssetCreator(techset::CreatePixelShaderLoaderT5(memory, searchPath));

    auto loader = techset::CreateTechniqueCompilerT5(memory, zone, searchPath);

    SECTION("Can compile simple technique")
    {
        const auto [inputName, inputData] = GENERATE(Catch::Generators::table<const char*, const char*>({
            {"auto-create args", R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 3.0 "simple.hlsl"
  {
  }

  pixelShader 3.0 "simple.hlsl"
  {
  }

  vertex.position = code.position;
}
)TECHNIQUE"},
            {"manual args",      R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 3.0 "simple.hlsl"
  {
    worldMatrix = constant.worldMatrix;
    viewProjectionMatrix = constant.viewProjectionMatrix;
  }

  pixelShader 3.0 "simple.hlsl"
  {
  }

  vertex.position = code.position;
}
)TECHNIQUE"},
        }));

        CAPTURE(inputName);
        searchPath.AddFileData("techniques/pimp_technique_zprepass_example.tech", inputData);

        GivenVertexShaderFile("simple.hlsl", searchPath);
        GivenPixelShaderFile("simple.hlsl", searchPath);

        auto result = loader->CreateSubAsset("pimp_technique_zprepass_example", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "pimp_technique_zprepass_example"s);

        // Usually would be 0x80 set as well, but that's only set when postprocessing with materials
        CHECK(technique->flags == 0x04);

        REQUIRE(technique->passCount == 1);
        auto& pass = technique->passArray[0];
        CHECK(pass.customSamplerFlags == 0);

        REQUIRE(pass.vertexShader);
        CHECK(pass.vertexShader->name == "simple.hlsl"s);
        REQUIRE(pass.pixelShader);
        CHECK(pass.pixelShader->name == "simple.hlsl"s);

        REQUIRE(pass.vertexDecl);
        auto& vertexDecl = *pass.vertexDecl;
        CHECK(vertexDecl.hasOptionalSource == false);
        CHECK(vertexDecl.isLoaded == false);
        REQUIRE(vertexDecl.streamCount == 1);
        CHECK(vertexDecl.routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(vertexDecl.routing.data[0].dest == STREAM_DST_POSITION);

        REQUIRE(pass.perPrimArgCount == 1);
        REQUIRE(pass.perObjArgCount == 1);
        REQUIRE(pass.stableArgCount == 0);
        CHECK(pass.args[0].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[0].dest == 4);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX);
        CHECK(pass.args[0].u.codeConst.firstRow == 0);
        CHECK(pass.args[0].u.codeConst.rowCount == 4);

        CHECK(pass.args[1].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[1].dest == 0);
        CHECK(pass.args[1].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
        CHECK(pass.args[1].u.codeConst.firstRow == 0);
        CHECK(pass.args[1].u.codeConst.rowCount == 4);
    }

    SECTION("Can compile advanced technique")
    {
        const auto [inputName, inputData] = GENERATE(Catch::Generators::table<const char*, const char*>({
            {"auto-create args", R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 3.0 "advanced.hlsl"
  {
    Flicker_Min = material.Flicker_Min;
    Flicker_Max = material.Flicker_Max;
    Seed_Value = material.Seed_Value;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    Diffuse_MapSampler = material.Diffuse_MapSampler;
    Diffuse_Map_Damage = material.Diffuse_Map_Damage;
    Normal_Map_Cracked = material.Normal_Map_Cracked;
    Reveal_Map = material.Reveal_Map;
    Specular_Map = material.Specular_Map;
    Heat_Map = material.Heat_Map;
    Ember_Scale = material.Ember_Scale;
    Heat_Direction = material.Heat_Direction;
    Ember_Direction = material.Ember_Direction;
    Heat_Scale = material.Heat_Scale;
  }

  vertex.position = code.position;
  vertex.texcoord[0] = code.texcoord[0];
  vertex.normal = code.normal;
  vertex.texcoord[2] = code.tangent;
}
)TECHNIQUE"},
            {"manual args",      R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 3.0 "advanced.hlsl"
  {
    worldMatrix = constant.worldMatrix;
    baseLightingCoords = constant.baseLightingCoords;
    viewProjectionMatrix = constant.viewProjectionMatrix;
    shadowLookupMatrix = constant.shadowLookupMatrix;
    Flicker_Min = material.Flicker_Min;
    Flicker_Max = material.Flicker_Max;
    Seed_Value = material.Seed_Value;
    fogConsts = constant.fogConsts;
    fogConsts2 = constant.fogConsts2;
    gameTime = constant.gameTime;
    sunFogDir = constant.sunFogDir;
    sunFogColor = constant.sunFogColor;
    sunFog = constant.sunFog;
    fogColor = constant.fogColor;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    Diffuse_MapSampler = material.Diffuse_MapSampler;
    Diffuse_Map_Damage = material.Diffuse_Map_Damage;
    Normal_Map_Cracked = material.Normal_Map_Cracked;
    Reveal_Map = material.Reveal_Map;
    Specular_Map = material.Specular_Map;
    Heat_Map = material.Heat_Map;
    shadowmapSamplerSpot = sampler.shadowmapSamplerSpot;
    modelLightingSampler = sampler.modelLightingSampler;
    lightingLookupScale = constant.lightingLookupScale;
    glightPosXs = constant.glightPosXs;
    glightPosYs = constant.glightPosYs;
    glightPosZs = constant.glightPosZs;
    glightFallOffs = constant.glightFallOffs;
    glightReds = constant.glightReds;
    glightGreens = constant.glightGreens;
    glightBlues = constant.glightBlues;
    lightPosition = constant.lightPosition;
    lightDiffuse = constant.lightDiffuse;
    lightHeroScale = constant.lightHeroScale;
    lightSpotDir = constant.lightSpotDir;
    lightSpotFactors = constant.lightSpotFactors;
    lightAttenuation = constant.lightAttenuation;
    lightFallOffA = constant.lightFallOffA;
    spotShadowmapPixelAdjust = constant.spotShadowmapPixelAdjust;
    gameTime = constant.gameTime;
    hdrControl0 = constant.hdrControl0;
    heroLightingR = constant.heroLightingR;
    heroLightingG = constant.heroLightingG;
    heroLightingB = constant.heroLightingB;
    Ember_Scale = material.Ember_Scale;
    Heat_Direction = material.Heat_Direction;
    Ember_Direction = material.Ember_Direction;
    Heat_Scale = material.Heat_Scale;
  }

  vertex.position = code.position;
  vertex.texcoord[0] = code.texcoord[0];
  vertex.normal = code.normal;
  vertex.texcoord[2] = code.tangent;
}
)TECHNIQUE"},
        }));

        CAPTURE(inputName);
        searchPath.AddFileData("techniques/example_lit_omni_shadow_glight.tech", inputData);

        GivenVertexShaderFile("advanced.hlsl", searchPath);
        GivenPixelShaderFile("advanced.hlsl", searchPath);

        auto result = loader->CreateSubAsset("example_lit_omni_shadow_glight", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "example_lit_omni_shadow_glight"s);

        // Usually would be 0x80 set as well, but that's only set when postprocessing with materials
        CHECK(technique->flags == 0x10);

        REQUIRE(technique->passCount == 1);
        auto& pass = technique->passArray[0];
        CHECK(pass.customSamplerFlags == 1);

        REQUIRE(pass.vertexShader);
        CHECK(pass.vertexShader->name == "advanced.hlsl"s);
        REQUIRE(pass.pixelShader);
        CHECK(pass.pixelShader->name == "advanced.hlsl"s);

        REQUIRE(pass.vertexDecl);
        auto& vertexDecl = *pass.vertexDecl;
        CHECK(vertexDecl.hasOptionalSource == false);
        CHECK(vertexDecl.isLoaded == false);
        REQUIRE(vertexDecl.streamCount == 4);
        CHECK(vertexDecl.routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(vertexDecl.routing.data[0].dest == STREAM_DST_POSITION);
        CHECK(vertexDecl.routing.data[1].source == STREAM_SRC_TEXCOORD_0);
        CHECK(vertexDecl.routing.data[1].dest == STREAM_DST_TEXCOORD_0);
        CHECK(vertexDecl.routing.data[2].source == STREAM_SRC_NORMAL);
        CHECK(vertexDecl.routing.data[2].dest == STREAM_DST_NORMAL);
        CHECK(vertexDecl.routing.data[3].source == STREAM_SRC_TANGENT);
        CHECK(vertexDecl.routing.data[3].dest == STREAM_DST_TEXCOORD_2);

        REQUIRE(pass.perPrimArgCount == 2);
        REQUIRE(pass.perObjArgCount == 2);
        REQUIRE(pass.stableArgCount == 43);

        CHECK(pass.args[0].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[0].dest == 4);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX);
        CHECK(pass.args[0].u.codeConst.firstRow == 0);
        CHECK(pass.args[0].u.codeConst.rowCount == 3);

        CHECK(pass.args[1].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[1].dest == 8);
        CHECK(pass.args[1].u.codeConst.index == CONST_SRC_CODE_BASE_LIGHTING_COORDS);
        CHECK(pass.args[1].u.codeConst.firstRow == 0);
        CHECK(pass.args[1].u.codeConst.rowCount == 1);

        CHECK(pass.args[2].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[2].dest == 0);
        CHECK(pass.args[2].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
        CHECK(pass.args[2].u.codeConst.firstRow == 0);
        CHECK(pass.args[2].u.codeConst.rowCount == 4);

        CHECK(pass.args[3].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[3].dest == 24);
        CHECK(pass.args[3].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX);
        CHECK(pass.args[3].u.codeConst.firstRow == 0);
        CHECK(pass.args[3].u.codeConst.rowCount == 4);

        CHECK(pass.args[4].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[4].dest == 36);
        CHECK(pass.args[4].u.nameHash == 0x2DDF50E9);

        CHECK(pass.args[5].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[5].dest == 23);
        CHECK(pass.args[5].u.nameHash == 0x2DDF51F7);

        CHECK(pass.args[6].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[6].dest == 37);
        CHECK(pass.args[6].u.nameHash == 0x9CC05A63);

        CHECK(pass.args[7].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[7].dest == 5);
        CHECK(pass.args[7].u.nameHash == 0x25D709F9);

        CHECK(pass.args[8].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[8].dest == 7);
        CHECK(pass.args[8].u.nameHash == 0x96F1B7B9);

        CHECK(pass.args[9].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[9].dest == 3);
        CHECK(pass.args[9].u.nameHash == 0xCFBF1DD6);

        CHECK(pass.args[10].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[10].dest == 2);
        CHECK(pass.args[10].u.nameHash == 0xCFED92EA);

        CHECK(pass.args[11].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[11].dest == 4);
        CHECK(pass.args[11].u.nameHash == 0xD28C20CC);

        CHECK(pass.args[12].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[12].dest == 6);
        CHECK(pass.args[12].u.nameHash == 0xE4B9BF3B);

        CHECK(pass.args[13].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[13].dest == 20);
        CHECK(pass.args[13].u.codeConst.index == CONST_SRC_CODE_FOG);
        CHECK(pass.args[13].u.codeConst.firstRow == 0);
        CHECK(pass.args[13].u.codeConst.rowCount == 1);

        CHECK(pass.args[14].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[14].dest == 21);
        CHECK(pass.args[14].u.codeConst.index == CONST_SRC_CODE_FOG2);
        CHECK(pass.args[14].u.codeConst.firstRow == 0);
        CHECK(pass.args[14].u.codeConst.rowCount == 1);

        CHECK(pass.args[15].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[15].dest == 22);
        CHECK(pass.args[15].u.codeConst.index == CONST_SRC_CODE_GAMETIME);
        CHECK(pass.args[15].u.codeConst.firstRow == 0);
        CHECK(pass.args[15].u.codeConst.rowCount == 1);

        CHECK(pass.args[16].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[16].dest == 64);
        CHECK(pass.args[16].u.codeConst.index == CONST_SRC_CODE_SUN_FOG_DIR);
        CHECK(pass.args[16].u.codeConst.firstRow == 0);
        CHECK(pass.args[16].u.codeConst.rowCount == 1);

        CHECK(pass.args[17].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[17].dest == 65);
        CHECK(pass.args[17].u.codeConst.index == CONST_SRC_CODE_SUN_FOG_COLOR);
        CHECK(pass.args[17].u.codeConst.firstRow == 0);
        CHECK(pass.args[17].u.codeConst.rowCount == 1);

        CHECK(pass.args[18].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[18].dest == 66);
        CHECK(pass.args[18].u.codeConst.index == CONST_SRC_CODE_SUN_FOG);
        CHECK(pass.args[18].u.codeConst.firstRow == 0);
        CHECK(pass.args[18].u.codeConst.rowCount == 1);

        CHECK(pass.args[19].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[19].dest == 67);
        CHECK(pass.args[19].u.codeConst.index == CONST_SRC_CODE_FOG_COLOR);
        CHECK(pass.args[19].u.codeConst.firstRow == 0);
        CHECK(pass.args[19].u.codeConst.rowCount == 1);

        CHECK(pass.args[20].type == MTL_ARG_CODE_PIXEL_SAMPLER);
        CHECK(pass.args[20].dest == 1);
        CHECK(pass.args[20].u.codeSampler == TEXTURE_SRC_CODE_SHADOWMAP_SPOT);

        CHECK(pass.args[21].type == MTL_ARG_CODE_PIXEL_SAMPLER);
        CHECK(pass.args[21].dest == 11);
        CHECK(pass.args[21].u.codeSampler == TEXTURE_SRC_CODE_MODEL_LIGHTING);

        CHECK(pass.args[22].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[22].dest == 5);
        CHECK(pass.args[22].u.codeConst.index == CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE);
        CHECK(pass.args[22].u.codeConst.firstRow == 0);
        CHECK(pass.args[22].u.codeConst.rowCount == 1);

        CHECK(pass.args[23].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[23].dest == 6);
        CHECK(pass.args[23].u.codeConst.index == CONST_SRC_CODE_GLIGHT_POSXS);
        CHECK(pass.args[23].u.codeConst.firstRow == 0);
        CHECK(pass.args[23].u.codeConst.rowCount == 1);

        CHECK(pass.args[24].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[24].dest == 7);
        CHECK(pass.args[24].u.codeConst.index == CONST_SRC_CODE_GLIGHT_POSYS);
        CHECK(pass.args[24].u.codeConst.firstRow == 0);
        CHECK(pass.args[24].u.codeConst.rowCount == 1);

        CHECK(pass.args[25].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[25].dest == 8);
        CHECK(pass.args[25].u.codeConst.index == CONST_SRC_CODE_GLIGHT_POSZS);
        CHECK(pass.args[25].u.codeConst.firstRow == 0);
        CHECK(pass.args[25].u.codeConst.rowCount == 1);

        CHECK(pass.args[26].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[26].dest == 9);
        CHECK(pass.args[26].u.codeConst.index == CONST_SRC_CODE_GLIGHT_FALLOFFS);
        CHECK(pass.args[26].u.codeConst.firstRow == 0);
        CHECK(pass.args[26].u.codeConst.rowCount == 1);

        CHECK(pass.args[27].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[27].dest == 10);
        CHECK(pass.args[27].u.codeConst.index == CONST_SRC_CODE_GLIGHT_REDS);
        CHECK(pass.args[27].u.codeConst.firstRow == 0);
        CHECK(pass.args[27].u.codeConst.rowCount == 1);

        CHECK(pass.args[28].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[28].dest == 11);
        CHECK(pass.args[28].u.codeConst.index == CONST_SRC_CODE_GLIGHT_GREENS);
        CHECK(pass.args[28].u.codeConst.firstRow == 0);
        CHECK(pass.args[28].u.codeConst.rowCount == 1);

        CHECK(pass.args[29].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[29].dest == 20);
        CHECK(pass.args[29].u.codeConst.index == CONST_SRC_CODE_GLIGHT_BLUES);
        CHECK(pass.args[29].u.codeConst.firstRow == 0);
        CHECK(pass.args[29].u.codeConst.rowCount == 1);

        CHECK(pass.args[30].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[30].dest == 21);
        CHECK(pass.args[30].u.codeConst.index == CONST_SRC_CODE_LIGHT_POSITION);
        CHECK(pass.args[30].u.codeConst.firstRow == 0);
        CHECK(pass.args[30].u.codeConst.rowCount == 1);

        CHECK(pass.args[31].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[31].dest == 22);
        CHECK(pass.args[31].u.codeConst.index == CONST_SRC_CODE_LIGHT_DIFFUSE);
        CHECK(pass.args[31].u.codeConst.firstRow == 0);
        CHECK(pass.args[31].u.codeConst.rowCount == 1);

        CHECK(pass.args[32].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[32].dest == 23);
        CHECK(pass.args[32].u.codeConst.index == CONST_SRC_CODE_LIGHT_HERO_SCALE);
        CHECK(pass.args[32].u.codeConst.firstRow == 0);
        CHECK(pass.args[32].u.codeConst.rowCount == 1);

        CHECK(pass.args[33].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[33].dest == 24);
        CHECK(pass.args[33].u.codeConst.index == CONST_SRC_CODE_LIGHT_SPOTDIR);
        CHECK(pass.args[33].u.codeConst.firstRow == 0);
        CHECK(pass.args[33].u.codeConst.rowCount == 1);

        CHECK(pass.args[34].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[34].dest == 25);
        CHECK(pass.args[34].u.codeConst.index == CONST_SRC_CODE_LIGHT_SPOTFACTORS);
        CHECK(pass.args[34].u.codeConst.firstRow == 0);
        CHECK(pass.args[34].u.codeConst.rowCount == 1);

        CHECK(pass.args[35].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[35].dest == 26);
        CHECK(pass.args[35].u.codeConst.index == CONST_SRC_CODE_LIGHT_ATTENUATION);
        CHECK(pass.args[35].u.codeConst.firstRow == 0);
        CHECK(pass.args[35].u.codeConst.rowCount == 1);

        CHECK(pass.args[36].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[36].dest == 27);
        CHECK(pass.args[36].u.codeConst.index == CONST_SRC_CODE_LIGHT_FALLOFF_A);
        CHECK(pass.args[36].u.codeConst.firstRow == 0);
        CHECK(pass.args[36].u.codeConst.rowCount == 1);

        CHECK(pass.args[37].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[37].dest == 28);
        CHECK(pass.args[37].u.codeConst.index == CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST);
        CHECK(pass.args[37].u.codeConst.firstRow == 0);
        CHECK(pass.args[37].u.codeConst.rowCount == 1);

        CHECK(pass.args[38].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[38].dest == 29);
        CHECK(pass.args[38].u.codeConst.index == CONST_SRC_CODE_GAMETIME);
        CHECK(pass.args[38].u.codeConst.firstRow == 0);
        CHECK(pass.args[38].u.codeConst.rowCount == 1);

        CHECK(pass.args[39].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[39].dest == 30);
        CHECK(pass.args[39].u.codeConst.index == CONST_SRC_CODE_HDRCONTROL_0);
        CHECK(pass.args[39].u.codeConst.firstRow == 0);
        CHECK(pass.args[39].u.codeConst.rowCount == 1);

        CHECK(pass.args[40].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[40].dest == 31);
        CHECK(pass.args[40].u.codeConst.index == CONST_SRC_CODE_HERO_LIGHTING_R);
        CHECK(pass.args[40].u.codeConst.firstRow == 0);
        CHECK(pass.args[40].u.codeConst.rowCount == 1);

        CHECK(pass.args[41].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[41].dest == 32);
        CHECK(pass.args[41].u.codeConst.index == CONST_SRC_CODE_HERO_LIGHTING_G);
        CHECK(pass.args[41].u.codeConst.firstRow == 0);
        CHECK(pass.args[41].u.codeConst.rowCount == 1);

        CHECK(pass.args[42].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[42].dest == 33);
        CHECK(pass.args[42].u.codeConst.index == CONST_SRC_CODE_HERO_LIGHTING_B);
        CHECK(pass.args[42].u.codeConst.firstRow == 0);
        CHECK(pass.args[42].u.codeConst.rowCount == 1);

        CHECK(pass.args[43].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[43].dest == 34);
        CHECK(pass.args[43].u.nameHash == 0x349EB03A);

        CHECK(pass.args[44].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[44].dest == 36);
        CHECK(pass.args[44].u.nameHash == 0x978B6822);

        CHECK(pass.args[45].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[45].dest == 35);
        CHECK(pass.args[45].u.nameHash == 0xF6410F67);

        CHECK(pass.args[46].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[46].dest == 37);
        CHECK(pass.args[46].u.nameHash == 0xFA98347F);
    }
}
