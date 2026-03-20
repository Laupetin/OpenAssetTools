#include "Game/IW4/Techset/TechniqueCompilerIW4.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Techset/PixelShaderLoaderIW4.h"
#include "Game/IW4/Techset/VertexDeclCompilerIW4.h"
#include "Game/IW4/Techset/VertexShaderLoaderIW4.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Shader/ShaderCommon.h"
#include "Utils/MemoryManager.h"
#include "catch2/generators/catch_generators.hpp"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>

using namespace IW4;
using namespace Catch;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    void GivenVertexShaderFile(const std::string& name, MockSearchPath& searchPath)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/IW4/Techset" / std::format("vs_{}.cso", name);
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/IW4/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        std::string data(fileSize, '\0');
        file.read(data.data(), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        searchPath.AddFileData(shader::GetFileNameForPixelShaderAssetName(name), std::move(data));
    }
} // namespace

TEST_CASE("TechniqueCompilerIW4", "[iw4][techset][compiler]")
{

    Zone zone("MockZone", 0, GameId::IW4, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
    MockSearchPath searchPath;

    creatorCollection.AddAssetCreator(techset::CreateVertexDeclCompilerIW4(memory));
    creatorCollection.AddAssetCreator(techset::CreateVertexShaderLoaderIW4(memory, searchPath));
    creatorCollection.AddAssetCreator(techset::CreatePixelShaderLoaderIW4(memory, searchPath));

    auto loader = techset::CreateTechniqueCompilerIW4(memory, zone, searchPath);

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
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    normalMapSampler = material.normalMap;
    colorMapSampler = material.colorMap;
    fogColorLinear = float4(0, 0, 0, 0);
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
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
    viewProjectionMatrix = constant.viewProjectionMatrix;
    fogConsts = constant.fogConsts;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    attenuationSampler = sampler.attenuationSampler;
    normalMapSampler = material.normalMap;
    colorMapSampler = material.colorMap;
    lightPosition = constant.lightPosition;
    lightDiffuse = constant.lightDiffuse;
    fogColorLinear = float4(0, 0, 0, 0);
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
  vertex.texcoord[0] = code.texcoord[0];
  vertex.normal = code.normal;
  vertex.texcoord[2] = code.tangent;
}
)TECHNIQUE"},
        }));

        CAPTURE(inputName);
        searchPath.AddFileData("techniques/example_lit_omni.tech", inputData);

        GivenVertexShaderFile("advanced.hlsl", searchPath);
        GivenPixelShaderFile("advanced.hlsl", searchPath);

        auto result = loader->CreateSubAsset("example_lit_omni", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "example_lit_omni"s);

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
        REQUIRE(vertexDecl.streamCount == 5);
        CHECK(vertexDecl.routing.data[0].source == STREAM_SRC_POSITION);
        CHECK(vertexDecl.routing.data[0].dest == STREAM_DST_POSITION);
        CHECK(vertexDecl.routing.data[1].source == STREAM_SRC_COLOR);
        CHECK(vertexDecl.routing.data[1].dest == STREAM_DST_COLOR_0);
        CHECK(vertexDecl.routing.data[2].source == STREAM_SRC_TEXCOORD_0);
        CHECK(vertexDecl.routing.data[2].dest == STREAM_DST_TEXCOORD_0);
        CHECK(vertexDecl.routing.data[3].source == STREAM_SRC_NORMAL);
        CHECK(vertexDecl.routing.data[3].dest == STREAM_DST_NORMAL);
        CHECK(vertexDecl.routing.data[4].source == STREAM_SRC_TANGENT);
        CHECK(vertexDecl.routing.data[4].dest == STREAM_DST_TEXCOORD_2);

        REQUIRE(pass.perPrimArgCount == 1);
        REQUIRE(pass.perObjArgCount == 2);
        REQUIRE(pass.stableArgCount == 6);

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

        CHECK(pass.args[2].type == MTL_ARG_CODE_PIXEL_SAMPLER);
        CHECK(pass.args[2].dest == 5);
        CHECK(pass.args[2].u.codeSampler == TEXTURE_SRC_CODE_LIGHT_ATTENUATION);

        CHECK(pass.args[3].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[3].dest == 4);
        CHECK(pass.args[3].u.nameHash == 0x59D30D0F);

        CHECK(pass.args[4].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[4].dest == 0);
        CHECK(pass.args[4].u.nameHash == 0xA0AB1041);

        CHECK(pass.args[5].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[5].dest == 21);
        CHECK(pass.args[5].u.codeConst.index == CONST_SRC_CODE_FOG);
        CHECK(pass.args[5].u.codeConst.firstRow == 0);
        CHECK(pass.args[5].u.codeConst.rowCount == 1);

        CHECK(pass.args[6].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[6].dest == 17);
        CHECK(pass.args[6].u.codeConst.index == CONST_SRC_CODE_LIGHT_POSITION);
        CHECK(pass.args[6].u.codeConst.firstRow == 0);
        CHECK(pass.args[6].u.codeConst.rowCount == 1);

        CHECK(pass.args[7].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[7].dest == 18);
        CHECK(pass.args[7].u.codeConst.index == CONST_SRC_CODE_LIGHT_DIFFUSE);
        CHECK(pass.args[7].u.codeConst.firstRow == 0);
        CHECK(pass.args[7].u.codeConst.rowCount == 1);

        CHECK(pass.args[8].type == MTL_ARG_LITERAL_PIXEL_CONST);
        CHECK(pass.args[8].dest == 0);
        CHECK((*pass.args[8].u.literalConst)[0] == 0.0f);
        CHECK((*pass.args[8].u.literalConst)[1] == 0.0f);
        CHECK((*pass.args[8].u.literalConst)[2] == 0.0f);
        CHECK((*pass.args[8].u.literalConst)[3] == 0.0f);
    }
}
