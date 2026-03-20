#include "Game/IW5/Techset/TechniqueCompilerIW5.h"

#include "Game/IW5/IW5.h"
#include "Game/IW5/Techset/PixelShaderLoaderIW5.h"
#include "Game/IW5/Techset/VertexDeclCompilerIW5.h"
#include "Game/IW5/Techset/VertexShaderLoaderIW5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Shader/ShaderCommon.h"
#include "Utils/MemoryManager.h"
#include "catch2/generators/catch_generators.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>

using namespace IW5;
using namespace Catch;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    void GivenVertexShaderFile(const std::string& name, MockSearchPath& searchPath)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/IW5/Techset" / std::format("vs_{}.cso", name);
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/IW5/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        std::string data(fileSize, '\0');
        file.read(data.data(), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        searchPath.AddFileData(shader::GetFileNameForPixelShaderAssetName(name), std::move(data));
    }
} // namespace

TEST_CASE("TechniqueCompilerIW5", "[iw5][techset][compiler]")
{

    Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
    MockSearchPath searchPath;

    creatorCollection.AddAssetCreator(techset::CreateVertexDeclCompilerIW5(memory));
    creatorCollection.AddAssetCreator(techset::CreateVertexShaderLoaderIW5(memory, searchPath));
    creatorCollection.AddAssetCreator(techset::CreatePixelShaderLoaderIW5(memory, searchPath));

    auto loader = techset::CreateTechniqueCompilerIW5(memory, zone, searchPath);

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
        searchPath.AddFileData("techniques/zprepass.tech", inputData);

        GivenVertexShaderFile("simple.hlsl", searchPath);
        GivenPixelShaderFile("simple.hlsl", searchPath);

        auto result = loader->CreateSubAsset("zprepass", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "zprepass"s);

        CHECK(technique->flags == 0x204);

        REQUIRE(technique->passCount == 1);
        auto& pass = technique->passArray[0];
        CHECK(pass.customSamplerFlags == 0);

        REQUIRE(pass.vertexShader);
        CHECK(pass.vertexShader->name == "simple.hlsl"s);
        REQUIRE(pass.pixelShader);
        CHECK(pass.pixelShader->name == "simple.hlsl"s);

        REQUIRE(pass.vertexDecl);
        auto& vertexDecl = *pass.vertexDecl;
        CHECK(vertexDecl.name == "pp"s);
        CHECK(vertexDecl.hasOptionalSource == false);
        REQUIRE(vertexDecl.streamCount == 1);
        CHECK(vertexDecl.routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(vertexDecl.routing.data[0].dest == STREAM_DST_POSITION);

        REQUIRE(pass.perPrimArgCount == 1);
        REQUIRE(pass.perObjArgCount == 1);
        REQUIRE(pass.stableArgCount == 0);
        CHECK(pass.args[0].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[0].dest == 4);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0);
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
    oceanUVAnimParmOctave0 = material.oceanUVAnimParmOctave0;
    oceanUVAnimParmOctave1 = material.oceanUVAnimParmOctave1;
    oceanAmplitude = material.oceanAmplitude;
    oceanDisplacementSampler = material.oceanDisplacementMap;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    oceanEnvSampler = material.oceanEnvMap;
    oceanDetailNormalSampler = material.oceanDetailNormalMap;
    oceanHeightNormalSampler = material.oceanHeightNormalMap;
    oceanFoamSampler = material.oceanFoamMap;
    oceanUVAnimParmFoam = material.oceanUVAnimParmFoam;
    envMapParms = material.envMapParms;
    oceanFoamParms = material.oceanFoamParms;
    oceanUVAnimParmOctave0 = material.oceanUVAnimParmOctave0;
    oceanUVAnimParmOctave1 = material.oceanUVAnimParmOctave1;
    oceanUVAnimParmDetail1 = material.oceanUVAnimParmDetail1;
    oceanUVAnimParmDetail0 = material.oceanUVAnimParmDetail0;
    oceanAmplitude = material.oceanAmplitude;
    oceanMiscParms = material.oceanMiscParms;
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
}
)TECHNIQUE"},
            {"manual args",      R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 3.0 "advanced.hlsl"
  {
    worldMatrix = constant.worldMatrix;
    viewProjectionMatrix = constant.viewProjectionMatrix;
    oceanUVAnimParmOctave0 = material.oceanUVAnimParmOctave0;
    oceanUVAnimParmOctave1 = material.oceanUVAnimParmOctave1;
    oceanAmplitude = material.oceanAmplitude;
    oceanDisplacementSampler = material.oceanDisplacementMap;
    eyeOffset = constant.eyeOffset;
    fogConsts = constant.fogConsts;
    gameTime = constant.gameTime;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    oceanEnvSampler = material.oceanEnvMap;
    oceanDetailNormalSampler = material.oceanDetailNormalMap;
    oceanHeightNormalSampler = material.oceanHeightNormalMap;
    oceanFoamSampler = material.oceanFoamMap;
    fogColorLinear = constant.fogColorLinear;
    gameTime = constant.gameTime;
    oceanUVAnimParmFoam = material.oceanUVAnimParmFoam;
    envMapParms = material.envMapParms;
    oceanFoamParms = material.oceanFoamParms;
    oceanUVAnimParmOctave0 = material.oceanUVAnimParmOctave0;
    oceanUVAnimParmOctave1 = material.oceanUVAnimParmOctave1;
    oceanUVAnimParmDetail1 = material.oceanUVAnimParmDetail1;
    oceanUVAnimParmDetail0 = material.oceanUVAnimParmDetail0;
    oceanAmplitude = material.oceanAmplitude;
    oceanMiscParms = material.oceanMiscParms;
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
}
)TECHNIQUE"},
        }));

        CAPTURE(inputName);
        searchPath.AddFileData("techniques/example_lit_sun_shadow.tech", inputData);

        GivenVertexShaderFile("advanced.hlsl", searchPath);
        GivenPixelShaderFile("advanced.hlsl", searchPath);

        auto result = loader->CreateSubAsset("example_lit_sun_shadow", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "example_lit_sun_shadow"s);

        CHECK(technique->flags == 0);

        REQUIRE(technique->passCount == 1);
        auto& pass = technique->passArray[0];
        CHECK(pass.customSamplerFlags == 0);

        REQUIRE(pass.vertexShader);
        CHECK(pass.vertexShader->name == "advanced.hlsl"s);
        REQUIRE(pass.pixelShader);
        CHECK(pass.pixelShader->name == "advanced.hlsl"s);

        REQUIRE(pass.vertexDecl);
        auto& vertexDecl = *pass.vertexDecl;
        CHECK(vertexDecl.hasOptionalSource == false);
        REQUIRE(vertexDecl.streamCount == 2);
        CHECK(vertexDecl.routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(vertexDecl.routing.data[0].dest == STREAM_DST_POSITION);
        CHECK(vertexDecl.routing.data[1].source == STREAM_SRC_COLOR);
        CHECK(vertexDecl.routing.data[1].dest == STREAM_DST_COLOR_0);

        REQUIRE(pass.perPrimArgCount == 1);
        REQUIRE(pass.perObjArgCount == 1);
        REQUIRE(pass.stableArgCount == 22);

        CHECK(pass.args[0].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[0].dest == 4);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0);
        CHECK(pass.args[0].u.codeConst.firstRow == 0);
        CHECK(pass.args[0].u.codeConst.rowCount == 4);

        CHECK(pass.args[1].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[1].dest == 0);
        CHECK(pass.args[1].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
        CHECK(pass.args[1].u.codeConst.firstRow == 0);
        CHECK(pass.args[1].u.codeConst.rowCount == 4);

        CHECK(pass.args[2].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[2].dest == 59);
        CHECK(pass.args[2].u.nameHash == 0x470F6C9A);

        CHECK(pass.args[3].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[3].dest == 60);
        CHECK(pass.args[3].u.nameHash == 0x470F6C9B);

        CHECK(pass.args[4].type == MTL_ARG_MATERIAL_VERTEX_CONST);
        CHECK(pass.args[4].dest == 58);
        CHECK(pass.args[4].u.nameHash == 0x9D5408FF);

        CHECK(pass.args[5].type == MTL_ARG_MATERIAL_VERTEX_SAMPLER);
        CHECK(pass.args[5].dest == 2);
        CHECK(pass.args[5].u.nameHash == 0x29F357AD);

        CHECK(pass.args[6].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[6].dest == 5);
        CHECK(pass.args[6].u.nameHash == 0x7D392967);

        CHECK(pass.args[7].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[7].dest == 7);
        CHECK(pass.args[7].u.nameHash == 0x88792E38);

        CHECK(pass.args[8].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[8].dest == 4);
        CHECK(pass.args[8].u.nameHash == 0x8CB95536);

        CHECK(pass.args[9].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[9].dest == 6);
        CHECK(pass.args[9].u.nameHash == 0xC096573F);

        CHECK(pass.args[10].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[10].dest == 10);
        CHECK(pass.args[10].u.codeConst.index == CONST_SRC_CODE_EYEOFFSET);
        CHECK(pass.args[10].u.codeConst.firstRow == 0);
        CHECK(pass.args[10].u.codeConst.rowCount == 1);

        CHECK(pass.args[11].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[11].dest == 21);
        CHECK(pass.args[11].u.codeConst.index == CONST_SRC_CODE_FOG);
        CHECK(pass.args[11].u.codeConst.firstRow == 0);
        CHECK(pass.args[11].u.codeConst.rowCount == 1);

        CHECK(pass.args[12].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[12].dest == 22);
        CHECK(pass.args[12].u.codeConst.index == CONST_SRC_CODE_GAMETIME);
        CHECK(pass.args[12].u.codeConst.firstRow == 0);
        CHECK(pass.args[12].u.codeConst.rowCount == 1);

        CHECK(pass.args[13].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[13].dest == 0);
        CHECK(pass.args[13].u.codeConst.index == CONST_SRC_CODE_FOG_COLOR_LINEAR);
        CHECK(pass.args[13].u.codeConst.firstRow == 0);
        CHECK(pass.args[13].u.codeConst.rowCount == 1);

        CHECK(pass.args[14].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[14].dest == 3);
        CHECK(pass.args[14].u.codeConst.index == CONST_SRC_CODE_GAMETIME);
        CHECK(pass.args[14].u.codeConst.firstRow == 0);
        CHECK(pass.args[14].u.codeConst.rowCount == 1);

        CHECK(pass.args[15].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[15].dest == 9);
        CHECK(pass.args[15].u.nameHash == 0x64E3AE5);

        CHECK(pass.args[16].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[16].dest == 5);
        CHECK(pass.args[16].u.nameHash == 0x3D9994DC);

        CHECK(pass.args[17].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[17].dest == 22);
        CHECK(pass.args[17].u.nameHash == 0x3FC0F1DE);

        CHECK(pass.args[18].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[18].dest == 7);
        CHECK(pass.args[18].u.nameHash == 0x470F6C9A);

        CHECK(pass.args[19].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[19].dest == 8);
        CHECK(pass.args[19].u.nameHash == 0x470F6C9B);

        CHECK(pass.args[20].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[20].dest == 20);
        CHECK(pass.args[20].u.nameHash == 0x6373ABA0);

        CHECK(pass.args[21].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[21].dest == 11);
        CHECK(pass.args[21].u.nameHash == 0x6373ABA1);

        CHECK(pass.args[22].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[22].dest == 6);
        CHECK(pass.args[22].u.nameHash == 0x9D5408FF);

        CHECK(pass.args[23].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[23].dest == 21);
        CHECK(pass.args[23].u.nameHash == 0xAA2E7C4F);
    }
}
