#include "Game/T6/Techset/TechniqueCompilerT6.h"

#include "Game/T6/T6.h"
#include "Game/T6/Techset/PixelShaderLoaderT6.h"
#include "Game/T6/Techset/VertexDeclCompilerT6.h"
#include "Game/T6/Techset/VertexShaderLoaderT6.h"
#include "OatTestPaths.h"
#include "SearchPath/MockSearchPath.h"
#include "Shader/ShaderCommon.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>

using namespace T6;
using namespace Catch;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    void GivenVertexShaderFile(const std::string& name, MockSearchPath& searchPath)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/T6/Techset" / std::format("vs_{}.cso", name);
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjCompilingTests/Game/T6/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        std::string data(fileSize, '\0');
        file.read(data.data(), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        searchPath.AddFileData(shader::GetFileNameForPixelShaderAssetName(name), std::move(data));
    }
} // namespace

TEST_CASE("TechniqueCompilerT6", "[t6][techset][compiler]")
{

    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    AssetCreatorCollection creatorCollection(zone);
    IgnoredAssetLookup ignoredAssetLookup;
    AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);
    MockSearchPath searchPath;

    creatorCollection.AddSubAssetCreator(techset::CreateVertexDeclCompilerT6(memory));
    creatorCollection.AddSubAssetCreator(techset::CreateVertexShaderLoaderT6(memory, searchPath));
    creatorCollection.AddSubAssetCreator(techset::CreatePixelShaderLoaderT6(memory, searchPath));

    auto loader = techset::CreateTechniqueCompilerT6(memory, searchPath);

    SECTION("Can compile simple technique")
    {
        searchPath.AddFileData("techniques/pimp_technique_zprepass_example.tech", R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 4.0 "simple.hlsl"
  {
  }

  pixelShader 4.0 "simple.hlsl"
  {
  }

  vertex.position = code.position;
}
)TECHNIQUE");

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

        // idk
        // CHECK(pass.precompiledIndex == VERTEX_SHADER_MODEL_UNLIT);

        // Set from techset
        // CHECK(pass.materialType == MTL_TYPE_MODEL_VERTCOL);

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
        CHECK(pass.args[0].location.offset == 0);
        CHECK(pass.args[0].size == 0x40);
        CHECK(pass.args[0].buffer == 3);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX);
        CHECK(pass.args[0].u.codeConst.firstRow == 0);
        CHECK(pass.args[0].u.codeConst.rowCount == 4);

        CHECK(pass.args[1].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[1].location.offset == 0x240);
        CHECK(pass.args[1].size == 0x40);
        CHECK(pass.args[1].buffer == 0);
        CHECK(pass.args[1].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
        CHECK(pass.args[1].u.codeConst.firstRow == 0);
        CHECK(pass.args[1].u.codeConst.rowCount == 4);
    }

    SECTION("Can compile advanced technique")
    {
        searchPath.AddFileData("techniques/example_lit_sun_shadow.tech", R"TECHNIQUE(
{
  stateMap "passthrough";

  vertexShader 4.0 "advanced.hlsl"
  {
  }

  pixelShader 4.0 "advanced.hlsl"
  {
    normalMapSampler = material.normalMap;
    normalMapSampler1 = material.normalMap1;
    colorMapSampler = material.colorMap;
    colorMapSampler1 = material.colorMap1;
    colorMapSampler3 = material.colorMap3;
    colorMapSampler2 = material.colorMap2;
    alphaRevealParms1 = material.alphaRevealParms1;
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
  vertex.texcoord[0] = code.texcoord[0];
  vertex.normal = code.normal;
  vertex.texcoord[2] = code.tangent;
  vertex.texcoord[1] = code.texcoord[1];
  vertex.texcoord[3] = code.texcoord[2];
  vertex.texcoord[4] = code.texcoord[3];
  vertex.texcoord[5] = code.normalTransform[0];
}
)TECHNIQUE");

        GivenVertexShaderFile("advanced.hlsl", searchPath);
        GivenPixelShaderFile("advanced.hlsl", searchPath);

        auto result = loader->CreateSubAsset("example_lit_sun_shadow", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<MaterialTechnique>*>(result.GetAssetInfo());
        const auto* technique = assetInfo->Asset();

        CHECK(technique->name == "example_lit_sun_shadow"s);

        // Usually would be 0x80 set as well, but that's only set when postprocessing with materials
        CHECK(technique->flags == 0x08);

        REQUIRE(technique->passCount == 1);
        auto& pass = technique->passArray[0];
        CHECK(pass.customSamplerFlags == 3);

        // idk
        // CHECK(pass.precompiledIndex == VERTEX_SHADER_NONE);

        // Set from techset
        // CHECK(pass.materialType == MTL_TYPE_DEFAULT);

        REQUIRE(pass.vertexShader);
        CHECK(pass.vertexShader->name == "advanced.hlsl"s);
        REQUIRE(pass.pixelShader);
        CHECK(pass.pixelShader->name == "advanced.hlsl"s);

        REQUIRE(pass.vertexDecl);
        auto& vertexDecl = *pass.vertexDecl;
        CHECK(vertexDecl.hasOptionalSource == true);
        CHECK(vertexDecl.isLoaded == false);
        REQUIRE(vertexDecl.streamCount == 9);
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
        CHECK(vertexDecl.routing.data[5].source == STREAM_SRC_TEXCOORD_1);
        CHECK(vertexDecl.routing.data[5].dest == STREAM_DST_TEXCOORD_1);
        CHECK(vertexDecl.routing.data[6].source == STREAM_SRC_TEXCOORD_2);
        CHECK(vertexDecl.routing.data[6].dest == STREAM_DST_TEXCOORD_3);
        CHECK(vertexDecl.routing.data[7].source == STREAM_SRC_TEXCOORD_3);
        CHECK(vertexDecl.routing.data[7].dest == STREAM_DST_TEXCOORD_4);
        CHECK(vertexDecl.routing.data[8].source == STREAM_SRC_NORMAL_TRANSFORM_0);
        CHECK(vertexDecl.routing.data[8].dest == STREAM_DST_TEXCOORD_5);

        REQUIRE(pass.perPrimArgCount == 1);
        REQUIRE(pass.perObjArgCount == 1);
        REQUIRE(pass.stableArgCount == 11);
        CHECK(pass.args[0].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[0].location.offset == 0);
        CHECK(pass.args[0].size == 0x40);
        CHECK(pass.args[0].buffer == 3);
        CHECK(pass.args[0].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX);
        CHECK(pass.args[0].u.codeConst.firstRow == 0);
        CHECK(pass.args[0].u.codeConst.rowCount == 4);

        CHECK(pass.args[1].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[1].location.offset == 0x240);
        CHECK(pass.args[1].size == 0x40);
        CHECK(pass.args[1].buffer == 0);
        CHECK(pass.args[1].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX);
        CHECK(pass.args[1].u.codeConst.firstRow == 0);
        CHECK(pass.args[1].u.codeConst.rowCount == 4);

        CHECK(pass.args[2].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[2].location.textureIndex == 1);
        CHECK(pass.args[2].location.samplerIndex == 1);
        CHECK(pass.args[2].size == 1);
        CHECK(pass.args[2].buffer == 0);
        CHECK(pass.args[2].u.nameHash == 0x59d30d0f);

        CHECK(pass.args[3].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[3].location.textureIndex == 3);
        CHECK(pass.args[3].location.samplerIndex == 5);
        CHECK(pass.args[3].size == 1);
        CHECK(pass.args[3].buffer == 0);
        CHECK(pass.args[3].u.nameHash == 0x9434aede);

        CHECK(pass.args[4].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[4].location.textureIndex == 0);
        CHECK(pass.args[4].location.samplerIndex == 0);
        CHECK(pass.args[4].size == 1);
        CHECK(pass.args[4].buffer == 0);
        CHECK(pass.args[4].u.nameHash == 0xa0ab1041);

        CHECK(pass.args[5].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[5].location.textureIndex == 2);
        CHECK(pass.args[5].location.samplerIndex == 2);
        CHECK(pass.args[5].size == 1);
        CHECK(pass.args[5].buffer == 0);
        CHECK(pass.args[5].u.nameHash == 0xb60d1850);

        CHECK(pass.args[6].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[6].location.textureIndex == 5);
        CHECK(pass.args[6].location.samplerIndex == 4);
        CHECK(pass.args[6].size == 1);
        CHECK(pass.args[6].buffer == 0);
        CHECK(pass.args[6].u.nameHash == 0xb60d1852);

        CHECK(pass.args[7].type == MTL_ARG_MATERIAL_PIXEL_SAMPLER);
        CHECK(pass.args[7].location.textureIndex == 4);
        CHECK(pass.args[7].location.samplerIndex == 3);
        CHECK(pass.args[7].size == 1);
        CHECK(pass.args[7].buffer == 0);
        CHECK(pass.args[7].u.nameHash == 0xb60d1853);

        CHECK(pass.args[8].type == MTL_ARG_CODE_VERTEX_CONST);
        CHECK(pass.args[8].location.offset == 0x300);
        CHECK(pass.args[8].size == 0x40);
        CHECK(pass.args[8].buffer == 0);
        CHECK(pass.args[8].u.codeConst.index == CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX);
        CHECK(pass.args[8].u.codeConst.firstRow == 0);
        CHECK(pass.args[8].u.codeConst.rowCount == 4);

        CHECK(pass.args[9].type == MTL_ARG_CODE_PIXEL_SAMPLER);
        CHECK(pass.args[9].location.textureIndex == 9);
        CHECK(pass.args[9].location.samplerIndex == 9);
        CHECK(pass.args[9].size == 1);
        CHECK(pass.args[9].buffer == 0);
        CHECK(pass.args[9].u.codeSampler == TEXTURE_SRC_CODE_SHADOWMAP_SUN);

        CHECK(pass.args[10].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[10].location.offset == 0x610);
        CHECK(pass.args[10].size == 0x10);
        CHECK(pass.args[10].buffer == 0);
        CHECK(pass.args[10].u.codeConst.index == CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION);
        CHECK(pass.args[10].u.codeConst.firstRow == 0);
        CHECK(pass.args[10].u.codeConst.rowCount == 1);

        CHECK(pass.args[11].type == MTL_ARG_CODE_PIXEL_CONST);
        CHECK(pass.args[11].location.offset == 0x630);
        CHECK(pass.args[11].size == 0x10);
        CHECK(pass.args[11].buffer == 0);
        CHECK(pass.args[11].u.codeConst.index == CONST_SRC_CODE_SUNSHADOWMAP_PIXEL_SIZE);
        CHECK(pass.args[11].u.codeConst.firstRow == 0);
        CHECK(pass.args[11].u.codeConst.rowCount == 1);

        CHECK(pass.args[12].type == MTL_ARG_MATERIAL_PIXEL_CONST);
        CHECK(pass.args[12].location.offset == 0x3b0);
        CHECK(pass.args[12].size == 0x10);
        CHECK(pass.args[12].buffer == 1);
        CHECK(pass.args[12].u.nameHash == 0x88befc31);
    }
}
