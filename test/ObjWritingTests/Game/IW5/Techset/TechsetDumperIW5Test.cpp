#include "Game/IW5/Techset/TechsetDumperIW5.h"

#include "Asset/AssetRegistration.h"
#include "Game/IW5/IW5.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

using namespace IW5;
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/IW5/Techset" / std::format("vs_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialVertexShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = reinterpret_cast<unsigned*>(memory.Alloc<char>(fileSize));
        shader->prog.loadDef.programSize = static_cast<decltype(GfxVertexShaderLoadDef::programSize)>(fileSize / 4u);
        file.read(reinterpret_cast<char*>(shader->prog.loadDef.program), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        return shader;
    }

    MaterialPixelShader* GivenPixelShader(const std::string& name, MemoryManager& memory)
    {
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/IW5/Techset" / std::format("ps_{}.cso", name);
        const auto fileSize = static_cast<size_t>(fs::file_size(filePath));

        std::ifstream file(filePath, std::ios::binary);
        REQUIRE(file.is_open());

        auto* shader = memory.Alloc<MaterialPixelShader>();
        shader->name = memory.Dup(name.c_str());
        shader->prog.loadDef.program = reinterpret_cast<unsigned*>(memory.Alloc<char>(fileSize));
        shader->prog.loadDef.programSize = static_cast<decltype(GfxVertexShaderLoadDef::programSize)>(fileSize / 4u);
        file.read(reinterpret_cast<char*>(shader->prog.loadDef.program), fileSize);
        REQUIRE(file.gcount() == static_cast<std::streamsize>(fileSize));

        return shader;
    }

    MaterialTechnique* GivenDepthPrepassTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_zprepass";
        technique->flags = 0x204;
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
        vertexDecl.name = "pp";
        vertexDecl.streamCount = 1;
        vertexDecl.hasOptionalSource = false;
        vertexDecl.routing.data[0].source = STREAM_SRC_POSITION;
        vertexDecl.routing.data[0].dest = STREAM_DST_POSITION;

        pass.args = memory.Alloc<MaterialShaderArgument>(2);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].dest = 4;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].dest = 0;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 4;

        return technique;
    }

    MaterialTechnique* GivenLitTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_lit";
        technique->flags = 0;
        technique->passCount = 1;

        auto& pass = technique->passArray[0];
        pass.perPrimArgCount = 1;
        pass.perObjArgCount = 1;
        pass.stableArgCount = 22;
        pass.customSamplerFlags = 0;

        pass.vertexShader = GivenVertexShader("advanced.hlsl", memory);
        pass.pixelShader = GivenPixelShader("advanced.hlsl", memory);

        pass.vertexDecl = memory.Alloc<MaterialVertexDeclaration>();
        auto& vertexDecl = *pass.vertexDecl;
        vertexDecl.name = "ppcc0";
        vertexDecl.streamCount = 2;
        vertexDecl.hasOptionalSource = false;
        vertexDecl.routing.data[0].source = STREAM_SRC_POSITION;
        vertexDecl.routing.data[0].dest = STREAM_DST_POSITION;
        vertexDecl.routing.data[1].source = STREAM_SRC_COLOR;
        vertexDecl.routing.data[1].dest = STREAM_DST_COLOR_0;

        pass.args = memory.Alloc<MaterialShaderArgument>(24);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].dest = 4;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

        pass.args[1].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[1].dest = 0;
        pass.args[1].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX;
        pass.args[1].u.codeConst.firstRow = 0;
        pass.args[1].u.codeConst.rowCount = 4;

        pass.args[2].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[2].dest = 59;
        pass.args[2].u.nameHash = 0x470F6C9A;

        pass.args[3].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[3].dest = 60;
        pass.args[3].u.nameHash = 0x470F6C9B;

        pass.args[4].type = MTL_ARG_MATERIAL_VERTEX_CONST;
        pass.args[4].dest = 58;
        pass.args[4].u.nameHash = 0x9D5408FF;

        pass.args[5].type = MTL_ARG_MATERIAL_VERTEX_SAMPLER;
        pass.args[5].dest = 2;
        pass.args[5].u.nameHash = 0x29F357AD;

        pass.args[6].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[6].dest = 5;
        pass.args[6].u.nameHash = 0x7D392967;

        pass.args[7].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[7].dest = 7;
        pass.args[7].u.nameHash = 0x88792E38;

        pass.args[8].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[8].dest = 4;
        pass.args[8].u.nameHash = 0x8CB95536;

        pass.args[9].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[9].dest = 6;
        pass.args[9].u.nameHash = 0xC096573F;

        pass.args[10].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[10].dest = 10;
        pass.args[10].u.codeConst.index = CONST_SRC_CODE_EYEOFFSET;
        pass.args[10].u.codeConst.firstRow = 0;
        pass.args[10].u.codeConst.rowCount = 1;

        pass.args[11].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[11].dest = 21;
        pass.args[11].u.codeConst.index = CONST_SRC_CODE_FOG;
        pass.args[11].u.codeConst.firstRow = 0;
        pass.args[11].u.codeConst.rowCount = 1;

        pass.args[12].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[12].dest = 22;
        pass.args[12].u.codeConst.index = CONST_SRC_CODE_GAMETIME;
        pass.args[12].u.codeConst.firstRow = 0;
        pass.args[12].u.codeConst.rowCount = 1;

        pass.args[13].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[13].dest = 0;
        pass.args[13].u.codeConst.index = CONST_SRC_CODE_FOG_COLOR_LINEAR;
        pass.args[13].u.codeConst.firstRow = 0;
        pass.args[13].u.codeConst.rowCount = 1;

        pass.args[14].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[14].dest = 3;
        pass.args[14].u.codeConst.index = CONST_SRC_CODE_GAMETIME;
        pass.args[14].u.codeConst.firstRow = 0;
        pass.args[14].u.codeConst.rowCount = 1;

        pass.args[15].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[15].dest = 9;
        pass.args[15].u.nameHash = 0x64E3AE5;

        pass.args[16].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[16].dest = 5;
        pass.args[16].u.nameHash = 0x3D9994DC;

        pass.args[17].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[17].dest = 22;
        pass.args[17].u.nameHash = 0x3FC0F1DE;

        pass.args[18].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[18].dest = 7;
        pass.args[18].u.nameHash = 0x470F6C9A;

        pass.args[19].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[19].dest = 8;
        pass.args[19].u.nameHash = 0x470F6C9B;

        pass.args[20].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[20].dest = 20;
        pass.args[20].u.nameHash = 0x6373ABA0;

        pass.args[21].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[21].dest = 11;
        pass.args[21].u.nameHash = 0x6373ABA1;

        pass.args[22].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[22].dest = 6;
        pass.args[22].u.nameHash = 0x9D5408FF;

        pass.args[23].type = MTL_ARG_MATERIAL_PIXEL_CONST;
        pass.args[23].dest = 21;
        pass.args[23].u.nameHash = 0xAA2E7C4F;

        return technique;
    }

    MaterialTechniqueSet* GivenTechset(Zone& zone, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = "example_techset";
        techset->worldVertFormat = MTL_WORLDVERT_TEX_4_NRM_2;

        techset->techniques[TECHNIQUE_DEPTH_PREPASS] = GivenDepthPrepassTechnique(memory);
        techset->techniques[TECHNIQUE_LIT] = GivenLitTechnique(memory);

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MaterialTechniqueSet>>(ASSET_TYPE_TECHNIQUE_SET, techset->name, techset));
        return techset;
    }
} // namespace

TEST_CASE("TechsetDumperIW5", "[iw5][techset][dumper]")
{
    Zone zone("MockZone", 0, GameId::IW5, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    MockSearchPath mockObjPath;
    MockOutputPath mockOutput;
    AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

    GivenTechset(zone, memory);

    techset::DumperIW5 dumper(true);

    SECTION("Can dump techset")
    {
        std::string expected(R"TECHSET(
"depth prepass":
  example_zprepass;

"lit":
  example_lit;
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
// TECHNIQUE FLAGS: 0x200
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
{
  stateMap "passthrough"; // TODO

  vertexShader 3.0 "advanced.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
    oceanUVAnimParmOctave0 = material.oceanUVAnimParmOctave0;
    oceanUVAnimParmOctave1 = material.oceanUVAnimParmOctave1;
    oceanAmplitude = material.oceanAmplitude;
    oceanDisplacementSampler = material.oceanDisplacementMap;
    // Omitted due to matching accessors: eyeOffset = constant.eyeOffset;
    // Omitted due to matching accessors: fogConsts = constant.fogConsts;
    // Omitted due to matching accessors: gameTime = constant.gameTime;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    oceanEnvSampler = material.oceanEnvMap;
    oceanDetailNormalSampler = material.oceanDetailNormalMap;
    oceanHeightNormalSampler = material.oceanHeightNormalMap;
    oceanFoamSampler = material.oceanFoamMap;
    // Omitted due to matching accessors: fogColorLinear = constant.fogColorLinear;
    // Omitted due to matching accessors: gameTime = constant.gameTime;
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
)TECHNIQUE");
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techniques/example_lit.tech");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }
}
