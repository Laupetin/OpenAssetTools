#include "Game/T6/Techset/TechsetDumperT6.h"

#include "Asset/AssetRegistration.h"
#include "Game/T6/GameT6.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

using namespace T6;
using namespace Catch;
using namespace std::literals;
namespace fs = std::filesystem;

namespace
{
    std::string Trimmed(const std::string& input)
    {
        auto start = input.find_first_not_of(" \r\n");
        if (start == std::string::npos)
            start = 0;

        auto end = input.find_last_not_of(" \r\n");
        if (end == std::string::npos)
            end = input.length();
        else
            end = end + 1;

        return input.substr(start, end - start);
    }

    MaterialVertexShader* GivenVertexShader(const std::string& name, MemoryManager& memory)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/T6/Techset" / std::format("vs_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialVertexShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = memory.Alloc<char>(fileSize);
        shader->prog.loadDef.programSize = static_cast<decltype(GfxVertexShaderLoadDef::programSize)>(fileSize);
        file.read(shader->prog.loadDef.program, fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        return shader;
    }

    MaterialPixelShader* GivenPixelShader(const std::string& name, MemoryManager& memory)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/T6/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialPixelShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = memory.Alloc<char>(fileSize);
        shader->prog.loadDef.programSize = static_cast<decltype(GfxPixelShaderLoadDef::programSize)>(fileSize);
        file.read(shader->prog.loadDef.program, fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        return shader;
    }

    MaterialTechnique* GivenDepthPrepassTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_zprepass";
        technique->flags = 0x84;
        technique->passCount = 1;

        auto& pass = technique->passArray[0];
        pass.perPrimArgCount = 1;
        pass.perObjArgCount = 1;
        pass.stableArgCount = 0;
        pass.customSamplerFlags = 0;
        pass.precompiledIndex = VERTEX_SHADER_MODEL_UNLIT;
        pass.materialType = MTL_TYPE_MODEL_VERTCOL;

        pass.vertexShader = GivenVertexShader("simple.hlsl", memory);
        pass.pixelShader = GivenPixelShader("simple.hlsl", memory);

        pass.vertexDecl = memory.Alloc<MaterialVertexDeclaration>();
        auto& vertexDecl = *pass.vertexDecl;
        vertexDecl.streamCount = 1;
        vertexDecl.hasOptionalSource = false;
        vertexDecl.isLoaded = false;
        vertexDecl.routing.data[0].source = STREAM_SRC_POSITION;
        vertexDecl.routing.data[0].dest = STREAM_DST_POSITION;

        pass.args = memory.Alloc<MaterialShaderArgument>(2);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].location.offset = 0;
        pass.args[0].size = 0x40;
        pass.args[0].buffer = 3;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].location.offset = 0x240;
        pass.args[1].size = 0x40;
        pass.args[1].buffer = 0;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 4;

        return technique;
    }

    MaterialTechnique* GivenLitSunShadowTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_lit_sun_shadow";
        technique->flags = 0x88;
        technique->passCount = 1;

        auto& pass = technique->passArray[0];
        pass.perPrimArgCount = 1;
        pass.perObjArgCount = 1;
        pass.stableArgCount = 11;
        pass.customSamplerFlags = 3;
        pass.precompiledIndex = VERTEX_SHADER_NONE;
        pass.materialType = MTL_TYPE_DEFAULT;

        pass.vertexShader = GivenVertexShader("advanced.hlsl", memory);
        pass.pixelShader = GivenPixelShader("advanced.hlsl", memory);

        pass.vertexDecl = memory.Alloc<MaterialVertexDeclaration>();
        auto& vertexDecl = *pass.vertexDecl;
        vertexDecl.streamCount = 9;
        vertexDecl.hasOptionalSource = true;
        vertexDecl.isLoaded = false;
        vertexDecl.routing.data[0].source = STREAM_SRC_POSITION;
        vertexDecl.routing.data[0].dest = STREAM_DST_POSITION;
        vertexDecl.routing.data[1].source = STREAM_SRC_COLOR;
        vertexDecl.routing.data[1].dest = STREAM_DST_COLOR_0;
        vertexDecl.routing.data[2].source = STREAM_SRC_TEXCOORD_0;
        vertexDecl.routing.data[2].dest = STREAM_DST_TEXCOORD_0;
        vertexDecl.routing.data[3].source = STREAM_SRC_NORMAL;
        vertexDecl.routing.data[3].dest = STREAM_DST_NORMAL;
        vertexDecl.routing.data[4].source = STREAM_SRC_TANGENT;
        vertexDecl.routing.data[4].dest = STREAM_DST_TEXCOORD_2;
        vertexDecl.routing.data[5].source = STREAM_SRC_TEXCOORD_1;
        vertexDecl.routing.data[5].dest = STREAM_DST_TEXCOORD_1;
        vertexDecl.routing.data[6].source = STREAM_SRC_TEXCOORD_2;
        vertexDecl.routing.data[6].dest = STREAM_DST_TEXCOORD_3;
        vertexDecl.routing.data[7].source = STREAM_SRC_TEXCOORD_3;
        vertexDecl.routing.data[7].dest = STREAM_DST_TEXCOORD_4;
        vertexDecl.routing.data[8].source = STREAM_SRC_NORMAL_TRANSFORM_0;
        vertexDecl.routing.data[8].dest = STREAM_DST_TEXCOORD_5;

        pass.args = memory.Alloc<MaterialShaderArgument>(13);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].location.offset = 0;
        pass.args[0].size = 0x40;
        pass.args[0].buffer = 3;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].location.offset = 0x240;
        pass.args[1].size = 0x40;
        pass.args[1].buffer = 0;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 4;

        pass.args[2].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[2].location.textureIndex = 1;
        pass.args[2].location.samplerIndex = 1;
        pass.args[2].size = 1;
        pass.args[2].buffer = 0;
        pass.args[2].u.nameHash = 0x59d30d0f;

        pass.args[3].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[3].location.textureIndex = 3;
        pass.args[3].location.samplerIndex = 5;
        pass.args[3].size = 1;
        pass.args[3].buffer = 0;
        pass.args[3].u.nameHash = 0x9434aede;

        pass.args[4].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[4].location.textureIndex = 0;
        pass.args[4].location.samplerIndex = 0;
        pass.args[4].size = 1;
        pass.args[4].buffer = 0;
        pass.args[4].u.nameHash = 0xa0ab1041;

        pass.args[5].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[5].location.textureIndex = 2;
        pass.args[5].location.samplerIndex = 2;
        pass.args[5].size = 1;
        pass.args[5].buffer = 0;
        pass.args[5].u.nameHash = 0xb60d1850;

        pass.args[6].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[6].location.textureIndex = 5;
        pass.args[6].location.samplerIndex = 4;
        pass.args[6].size = 1;
        pass.args[6].buffer = 0;
        pass.args[6].u.nameHash = 0xb60d1852;

        pass.args[7].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[7].location.textureIndex = 4;
        pass.args[7].location.samplerIndex = 3;
        pass.args[7].size = 1;
        pass.args[7].buffer = 0;
        pass.args[7].u.nameHash = 0xb60d1853;

        pass.args[8].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[8].location.offset = 0x300;
        pass.args[8].size = 0x40;
        pass.args[8].buffer = 0;
        pass.args[8].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX;
        pass.args[8].u.codeConst.firstRow = 0;
        pass.args[8].u.codeConst.rowCount = 4;

        pass.args[9].type = MTL_ARG_CODE_PIXEL_SAMPLER;
        pass.args[9].location.textureIndex = 9;
        pass.args[9].location.samplerIndex = 9;
        pass.args[9].size = 1;
        pass.args[9].buffer = 0;
        pass.args[9].u.codeSampler = TEXTURE_SRC_CODE_SHADOWMAP_SUN;

        pass.args[10].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[10].location.offset = 0x610;
        pass.args[10].size = 0x10;
        pass.args[10].buffer = 0;
        pass.args[10].u.codeConst.index = CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION;
        pass.args[10].u.codeConst.firstRow = 0;
        pass.args[10].u.codeConst.rowCount = 1;

        pass.args[11].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[11].location.offset = 0x630;
        pass.args[11].size = 0x10;
        pass.args[11].buffer = 0;
        pass.args[11].u.codeConst.index = CONST_SRC_CODE_SUNSHADOWMAP_PIXEL_SIZE;
        pass.args[11].u.codeConst.firstRow = 0;
        pass.args[11].u.codeConst.rowCount = 1;

        pass.args[12].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[12].location.offset = 0x3b0;
        pass.args[12].size = 0x10;
        pass.args[12].buffer = 1;
        pass.args[12].u.nameHash = 0x88befc31;

        return technique;
    }

    MaterialTechniqueSet* GivenTechset(Zone& zone, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = "example_techset";
        techset->worldVertFormat = MTL_WORLDVERT_TEX_4_NRM_2;

        techset->techniques[TECHNIQUE_DEPTH_PREPASS] = GivenDepthPrepassTechnique(memory);
        techset->techniques[TECHNIQUE_LIT_SUN_SHADOW] = GivenLitSunShadowTechnique(memory);

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MaterialTechniqueSet>>(ASSET_TYPE_TECHNIQUE_SET, techset->name, techset));
        return techset;
    }
} // namespace

TEST_CASE("TechsetDumperT6", "[t6][techset][dumper]")
{
    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    MockSearchPath mockObjPath;
    MockOutputPath mockOutput;
    AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

    GivenTechset(zone, memory);

    techset::DumperT6 dumper(true);

    SECTION("Can dump techset")
    {
        std::string expected(R"TECHSET(
"depth prepass":
  example_zprepass;

"lit sun shadow":
  example_lit_sun_shadow;
)TECHSET");

        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techsets/example_techset.techset");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }

    SECTION("Can dump simple technique")
    {
        std::string expected(R"TECHNIQUE(
// TECHNIQUE FLAGS: 0x4
// TECHNIQUE FLAGS: 0x80
{
  stateMap "passthrough"; // TODO

  vertexShader 4.0 "simple.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
  }

  pixelShader 4.0 "simple.hlsl"
  {
  }

  vertex.position = code.position;
}
)TECHNIQUE");
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techniques/example_zprepass.tech");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }

    SECTION("Can dump advanced technique")
    {
        std::string expected(R"TECHNIQUE(
// TECHNIQUE FLAGS: 0x8
// TECHNIQUE FLAGS: 0x80
{
  // CUSTOM SAMPLER FLAGS: 0x1
  // CUSTOM SAMPLER FLAGS: 0x2
  stateMap "passthrough"; // TODO

  vertexShader 4.0 "advanced.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
    // Omitted due to matching accessors: shadowLookupMatrix = constant.shadowLookupMatrix;
  }

  pixelShader 4.0 "advanced.hlsl"
  {
    normalMapSampler = material.normalMap;
    normalMapSampler1 = material.normalMap1;
    colorMapSampler = material.colorMap;
    colorMapSampler1 = material.colorMap1;
    colorMapSampler3 = material.colorMap3;
    colorMapSampler2 = material.colorMap2;
    // Omitted due to matching accessors: shadowmapSamplerSun = sampler.shadowmapSamplerSun;
    // Omitted due to matching accessors: shadowmapSwitchPartition = constant.shadowmapSwitchPartition;
    // Omitted due to matching accessors: sunShadowmapPixelSize = constant.sunShadowmapPixelSize;
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
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techniques/example_lit_sun_shadow.tech");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }
}
