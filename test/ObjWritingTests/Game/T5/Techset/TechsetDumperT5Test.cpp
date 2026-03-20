#include "Game/T5/Techset/TechsetDumperT5.h"

#include "Asset/AssetRegistration.h"
#include "Game/T5/GameT5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

using namespace T5;
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/T5/Techset" / std::format("vs_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialVertexShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = reinterpret_cast<unsigned*>(memory.Alloc<char>(fileSize));
        shader->prog.loadDef.programSize = static_cast<decltype(GfxVertexShaderLoadDef::programSize)>(fileSize / 4);
        file.read(reinterpret_cast<char*>(shader->prog.loadDef.program), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        return shader;
    }

    MaterialPixelShader* GivenPixelShader(const std::string& name, MemoryManager& memory)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/T5/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialPixelShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = reinterpret_cast<unsigned*>(memory.Alloc<char>(fileSize));
        shader->prog.loadDef.programSize = static_cast<decltype(GfxPixelShaderLoadDef::programSize)>(fileSize / 4);
        file.read(reinterpret_cast<char*>(shader->prog.loadDef.program), fileSize);
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
        pass.args[0].dest = 4;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].dest = 0;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 4;

        return technique;
    }

    MaterialTechnique* GivenLitOmniShadowGlightTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_lit_omni_shadow_glight";
        technique->flags = 0x90;
        technique->passCount = 1;

        auto& pass = technique->passArray[0];
        pass.perPrimArgCount = 2;
        pass.perObjArgCount = 2;
        pass.stableArgCount = 43;
        pass.customSamplerFlags = 1;

        pass.vertexShader = GivenVertexShader("advanced.hlsl", memory);
        pass.pixelShader = GivenPixelShader("advanced.hlsl", memory);

        pass.vertexDecl = memory.Alloc<MaterialVertexDeclaration>();
        auto& vertexDecl = *pass.vertexDecl;
        vertexDecl.streamCount = 4;
        vertexDecl.hasOptionalSource = false;
        vertexDecl.isLoaded = false;
        vertexDecl.routing.data[0].source = STREAM_SRC_POSITION;
        vertexDecl.routing.data[0].dest = STREAM_DST_POSITION;
        vertexDecl.routing.data[1].source = STREAM_SRC_TEXCOORD_0;
        vertexDecl.routing.data[1].dest = STREAM_DST_TEXCOORD_0;
        vertexDecl.routing.data[2].source = STREAM_SRC_NORMAL;
        vertexDecl.routing.data[2].dest = STREAM_DST_NORMAL;
        vertexDecl.routing.data[3].source = STREAM_SRC_TANGENT;
        vertexDecl.routing.data[3].dest = STREAM_DST_TEXCOORD_2;

        pass.args = memory.Alloc<MaterialShaderArgument>(47);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].dest = 4;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 3;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].dest = 8;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_BASE_LIGHTING_COORDS;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 1;

        pass.args[2].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[2].dest = 0;
        pass.args[2].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[2].u.codeConst.firstRow = 0;
        pass.args[2].u.codeConst.rowCount = 4;

        pass.args[3].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[3].dest = 24;
        pass.args[3].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX;
        pass.args[3].u.codeConst.firstRow = 0;
        pass.args[3].u.codeConst.rowCount = 4;

        pass.args[4].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[4].dest = 36;
        pass.args[4].u.nameHash = 0x2DDF50E9;

        pass.args[5].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[5].dest = 23;
        pass.args[5].u.nameHash = 0x2DDF51F7;

        pass.args[6].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[6].dest = 37;
        pass.args[6].u.nameHash = 0x9CC05A63;

        pass.args[7].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[7].dest = 5;
        pass.args[7].u.nameHash = 0x25D709F9;

        pass.args[8].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[8].dest = 7;
        pass.args[8].u.nameHash = 0x96F1B7B9;

        pass.args[9].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[9].dest = 3;
        pass.args[9].u.nameHash = 0xCFBF1DD6;

        pass.args[10].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[10].dest = 2;
        pass.args[10].u.nameHash = 0xCFED92EA;

        pass.args[11].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[11].dest = 4;
        pass.args[11].u.nameHash = 0xD28C20CC;

        pass.args[12].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[12].dest = 6;
        pass.args[12].u.nameHash = 0xE4B9BF3B;

        pass.args[13].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[13].dest = 20;
        pass.args[13].u.codeConst.index = CONST_SRC_CODE_FOG;
        pass.args[13].u.codeConst.firstRow = 0;
        pass.args[13].u.codeConst.rowCount = 1;

        pass.args[14].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[14].dest = 21;
        pass.args[14].u.codeConst.index = CONST_SRC_CODE_FOG2;
        pass.args[14].u.codeConst.firstRow = 0;
        pass.args[14].u.codeConst.rowCount = 1;

        pass.args[15].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[15].dest = 22;
        pass.args[15].u.codeConst.index = CONST_SRC_CODE_GAMETIME;
        pass.args[15].u.codeConst.firstRow = 0;
        pass.args[15].u.codeConst.rowCount = 1;

        pass.args[16].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[16].dest = 64;
        pass.args[16].u.codeConst.index = CONST_SRC_CODE_SUN_FOG_DIR;
        pass.args[16].u.codeConst.firstRow = 0;
        pass.args[16].u.codeConst.rowCount = 1;

        pass.args[17].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[17].dest = 65;
        pass.args[17].u.codeConst.index = CONST_SRC_CODE_SUN_FOG_COLOR;
        pass.args[17].u.codeConst.firstRow = 0;
        pass.args[17].u.codeConst.rowCount = 1;

        pass.args[18].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[18].dest = 66;
        pass.args[18].u.codeConst.index = CONST_SRC_CODE_SUN_FOG;
        pass.args[18].u.codeConst.firstRow = 0;
        pass.args[18].u.codeConst.rowCount = 1;

        pass.args[19].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[19].dest = 67;
        pass.args[19].u.codeConst.index = CONST_SRC_CODE_FOG_COLOR;
        pass.args[19].u.codeConst.firstRow = 0;
        pass.args[19].u.codeConst.rowCount = 1;

        pass.args[20].type = MTL_ARG_CODE_PIXEL_SAMPLER;
        pass.args[20].dest = 1;
        pass.args[20].u.codeSampler = TEXTURE_SRC_CODE_SHADOWMAP_SPOT;

        pass.args[21].type = MTL_ARG_CODE_PIXEL_SAMPLER;
        pass.args[21].dest = 11;
        pass.args[21].u.codeSampler = TEXTURE_SRC_CODE_MODEL_LIGHTING;

        pass.args[22].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[22].dest = 5;
        pass.args[22].u.codeConst.index = CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE;
        pass.args[22].u.codeConst.firstRow = 0;
        pass.args[22].u.codeConst.rowCount = 1;

        pass.args[23].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[23].dest = 6;
        pass.args[23].u.codeConst.index = CONST_SRC_CODE_GLIGHT_POSXS;
        pass.args[23].u.codeConst.firstRow = 0;
        pass.args[23].u.codeConst.rowCount = 1;

        pass.args[24].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[24].dest = 7;
        pass.args[24].u.codeConst.index = CONST_SRC_CODE_GLIGHT_POSYS;
        pass.args[24].u.codeConst.firstRow = 0;
        pass.args[24].u.codeConst.rowCount = 1;

        pass.args[25].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[25].dest = 8;
        pass.args[25].u.codeConst.index = CONST_SRC_CODE_GLIGHT_POSZS;
        pass.args[25].u.codeConst.firstRow = 0;
        pass.args[25].u.codeConst.rowCount = 1;

        pass.args[26].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[26].dest = 9;
        pass.args[26].u.codeConst.index = CONST_SRC_CODE_GLIGHT_FALLOFFS;
        pass.args[26].u.codeConst.firstRow = 0;
        pass.args[26].u.codeConst.rowCount = 1;

        pass.args[27].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[27].dest = 10;
        pass.args[27].u.codeConst.index = CONST_SRC_CODE_GLIGHT_REDS;
        pass.args[27].u.codeConst.firstRow = 0;
        pass.args[27].u.codeConst.rowCount = 1;

        pass.args[28].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[28].dest = 11;
        pass.args[28].u.codeConst.index = CONST_SRC_CODE_GLIGHT_GREENS;
        pass.args[28].u.codeConst.firstRow = 0;
        pass.args[28].u.codeConst.rowCount = 1;

        pass.args[29].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[29].dest = 20;
        pass.args[29].u.codeConst.index = CONST_SRC_CODE_GLIGHT_BLUES;
        pass.args[29].u.codeConst.firstRow = 0;
        pass.args[29].u.codeConst.rowCount = 1;

        pass.args[30].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[30].dest = 21;
        pass.args[30].u.codeConst.index = CONST_SRC_CODE_LIGHT_POSITION;
        pass.args[30].u.codeConst.firstRow = 0;
        pass.args[30].u.codeConst.rowCount = 1;

        pass.args[31].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[31].dest = 22;
        pass.args[31].u.codeConst.index = CONST_SRC_CODE_LIGHT_DIFFUSE;
        pass.args[31].u.codeConst.firstRow = 0;
        pass.args[31].u.codeConst.rowCount = 1;

        pass.args[32].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[32].dest = 23;
        pass.args[32].u.codeConst.index = CONST_SRC_CODE_LIGHT_HERO_SCALE;
        pass.args[32].u.codeConst.firstRow = 0;
        pass.args[32].u.codeConst.rowCount = 1;

        pass.args[33].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[33].dest = 24;
        pass.args[33].u.codeConst.index = CONST_SRC_CODE_LIGHT_SPOTDIR;
        pass.args[33].u.codeConst.firstRow = 0;
        pass.args[33].u.codeConst.rowCount = 1;

        pass.args[34].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[34].dest = 25;
        pass.args[34].u.codeConst.index = CONST_SRC_CODE_LIGHT_SPOTFACTORS;
        pass.args[34].u.codeConst.firstRow = 0;
        pass.args[34].u.codeConst.rowCount = 1;

        pass.args[35].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[35].dest = 26;
        pass.args[35].u.codeConst.index = CONST_SRC_CODE_LIGHT_ATTENUATION;
        pass.args[35].u.codeConst.firstRow = 0;
        pass.args[35].u.codeConst.rowCount = 1;

        pass.args[36].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[36].dest = 27;
        pass.args[36].u.codeConst.index = CONST_SRC_CODE_LIGHT_FALLOFF_A;
        pass.args[36].u.codeConst.firstRow = 0;
        pass.args[36].u.codeConst.rowCount = 1;

        pass.args[37].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[37].dest = 28;
        pass.args[37].u.codeConst.index = CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST;
        pass.args[37].u.codeConst.firstRow = 0;
        pass.args[37].u.codeConst.rowCount = 1;

        pass.args[38].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[38].dest = 29;
        pass.args[38].u.codeConst.index = CONST_SRC_CODE_GAMETIME;
        pass.args[38].u.codeConst.firstRow = 0;
        pass.args[38].u.codeConst.rowCount = 1;

        pass.args[39].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[39].dest = 30;
        pass.args[39].u.codeConst.index = CONST_SRC_CODE_HDRCONTROL_0;
        pass.args[39].u.codeConst.firstRow = 0;
        pass.args[39].u.codeConst.rowCount = 1;

        pass.args[40].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[40].dest = 31;
        pass.args[40].u.codeConst.index = CONST_SRC_CODE_HERO_LIGHTING_R;
        pass.args[40].u.codeConst.firstRow = 0;
        pass.args[40].u.codeConst.rowCount = 1;

        pass.args[41].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[41].dest = 32;
        pass.args[41].u.codeConst.index = CONST_SRC_CODE_HERO_LIGHTING_G;
        pass.args[41].u.codeConst.firstRow = 0;
        pass.args[41].u.codeConst.rowCount = 1;

        pass.args[42].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[42].dest = 33;
        pass.args[42].u.codeConst.index = CONST_SRC_CODE_HERO_LIGHTING_B;
        pass.args[42].u.codeConst.firstRow = 0;
        pass.args[42].u.codeConst.rowCount = 1;

        pass.args[43].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[43].dest = 34;
        pass.args[43].u.nameHash = 0x349EB03A;

        pass.args[44].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[44].dest = 36;
        pass.args[44].u.nameHash = 0x978B6822;

        pass.args[45].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[45].dest = 35;
        pass.args[45].u.nameHash = 0xF6410F67;

        pass.args[46].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[46].dest = 37;
        pass.args[46].u.nameHash = 0xFA98347F;

        return technique;
    }

    MaterialTechniqueSet* GivenTechset(Zone& zone, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = "example_techset";
        techset->worldVertFormat = MTL_WORLDVERT_TEX_4_NRM_2;

        techset->techniques[TECHNIQUE_DEPTH_PREPASS] = GivenDepthPrepassTechnique(memory);
        techset->techniques[TECHNIQUE_LIT_OMNI_SHADOW_GLIGHT] = GivenLitOmniShadowGlightTechnique(memory);

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MaterialTechniqueSet>>(ASSET_TYPE_TECHNIQUE_SET, techset->name, techset));
        return techset;
    }
} // namespace

TEST_CASE("TechsetDumperT5", "[t5][techset][dumper]")
{
    Zone zone("MockZone", 0, GameId::T6, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    MockSearchPath mockObjPath;
    MockOutputPath mockOutput;
    AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

    GivenTechset(zone, memory);

    techset::DumperT5 dumper(true);

    SECTION("Can dump techset")
    {
        std::string expected(R"TECHSET(
"depth prepass":
  example_zprepass;

"lit omni shadow glight":
  example_lit_omni_shadow_glight;
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

  vertexShader 3.0 "simple.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
  }

  pixelShader 3.0 "simple.hlsl"
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
// TECHNIQUE FLAGS: 0x10
// TECHNIQUE FLAGS: 0x80
{
  // CUSTOM SAMPLER FLAGS: 0x1
  stateMap "passthrough"; // TODO

  vertexShader 3.0 "advanced.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: baseLightingCoords = constant.baseLightingCoords;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
    // Omitted due to matching accessors: shadowLookupMatrix = constant.shadowLookupMatrix;
    Flicker_Min = material.Flicker_Min;
    Flicker_Max = material.Flicker_Max;
    Seed_Value = material.Seed_Value;
    // Omitted due to matching accessors: fogConsts = constant.fogConsts;
    // Omitted due to matching accessors: fogConsts2 = constant.fogConsts2;
    // Omitted due to matching accessors: gameTime = constant.gameTime;
    // Omitted due to matching accessors: sunFogDir = constant.sunFogDir;
    // Omitted due to matching accessors: sunFogColor = constant.sunFogColor;
    // Omitted due to matching accessors: sunFog = constant.sunFog;
    // Omitted due to matching accessors: fogColor = constant.fogColor;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    Diffuse_MapSampler = material.Diffuse_MapSampler;
    Diffuse_Map_Damage = material.Diffuse_Map_Damage;
    Normal_Map_Cracked = material.Normal_Map_Cracked;
    Reveal_Map = material.Reveal_Map;
    Specular_Map = material.Specular_Map;
    Heat_Map = material.Heat_Map;
    // Omitted due to matching accessors: shadowmapSamplerSpot = sampler.shadowmapSamplerSpot;
    // Omitted due to matching accessors: modelLightingSampler = sampler.modelLightingSampler;
    // Omitted due to matching accessors: lightingLookupScale = constant.lightingLookupScale;
    // Omitted due to matching accessors: glightPosXs = constant.glightPosXs;
    // Omitted due to matching accessors: glightPosYs = constant.glightPosYs;
    // Omitted due to matching accessors: glightPosZs = constant.glightPosZs;
    // Omitted due to matching accessors: glightFallOffs = constant.glightFallOffs;
    // Omitted due to matching accessors: glightReds = constant.glightReds;
    // Omitted due to matching accessors: glightGreens = constant.glightGreens;
    // Omitted due to matching accessors: glightBlues = constant.glightBlues;
    // Omitted due to matching accessors: lightPosition = constant.lightPosition;
    // Omitted due to matching accessors: lightDiffuse = constant.lightDiffuse;
    // Omitted due to matching accessors: lightHeroScale = constant.lightHeroScale;
    // Omitted due to matching accessors: lightSpotDir = constant.lightSpotDir;
    // Omitted due to matching accessors: lightSpotFactors = constant.lightSpotFactors;
    // Omitted due to matching accessors: lightAttenuation = constant.lightAttenuation;
    // Omitted due to matching accessors: lightFallOffA = constant.lightFallOffA;
    // Omitted due to matching accessors: spotShadowmapPixelAdjust = constant.spotShadowmapPixelAdjust;
    // Omitted due to matching accessors: gameTime = constant.gameTime;
    // Omitted due to matching accessors: hdrControl0 = constant.hdrControl0;
    // Omitted due to matching accessors: heroLightingR = constant.heroLightingR;
    // Omitted due to matching accessors: heroLightingG = constant.heroLightingG;
    // Omitted due to matching accessors: heroLightingB = constant.heroLightingB;
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
)TECHNIQUE");
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techniques/example_lit_omni_shadow_glight.tech");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }
}
