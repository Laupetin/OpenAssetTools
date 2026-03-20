#include "Game/IW3/Techset/TechsetDumperIW3.h"

#include "Asset/AssetRegistration.h"
#include "Game/IW3/IW3.h"
#include "OatTestPaths.h"
#include "SearchPath/MockOutputPath.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>

using namespace IW3;
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/IW3/Techset" / std::format("vs_{}.cso", name);
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
        const auto filePath = oat::paths::GetTestDirectory() / "ObjWritingTests/Game/IW3/Techset" / std::format("ps_{}.cso", name);
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
        technique->flags = 0x4;
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

    MaterialTechnique* GivenLitSpotTechnique(MemoryManager& memory)
    {
        auto* technique = memory.Alloc<MaterialTechnique>();
        technique->name = "example_lit_spot";
        technique->flags = 0x10;
        technique->passCount = 1;

        auto& pass = technique->passArray[0];
        pass.perPrimArgCount = 2;
        pass.perObjArgCount = 2;
        pass.stableArgCount = 10;
        pass.customSamplerFlags = 0;

        pass.vertexShader = GivenVertexShader("advanced.hlsl", memory);
        pass.pixelShader = GivenPixelShader("advanced.hlsl", memory);

        pass.vertexDecl = memory.Alloc<MaterialVertexDeclaration>();
        auto& vertexDecl = *pass.vertexDecl;
        vertexDecl.streamCount = 5;
        vertexDecl.hasOptionalSource = false;
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

        pass.args = memory.Alloc<MaterialShaderArgument>(14);
        pass.args[0].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[0].dest = 4;
        pass.args[0].u.codeConst.index = CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX;
        pass.args[0].u.codeConst.firstRow = 0;
        pass.args[0].u.codeConst.rowCount = 4;

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

        pass.args[3].type = MTL_ARG_CODE_PIXEL_SAMPLER;
        pass.args[3].dest = 6;
        pass.args[3].u.codeSampler = TEXTURE_SRC_CODE_LIGHT_ATTENUATION;

        pass.args[4].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[4].dest = 5;
        pass.args[4].u.nameHash = 0x59D30D0F;

        pass.args[5].type = MTL_ARG_MATERIAL_PIXEL_SAMPLER;
        pass.args[5].dest = 0;
        pass.args[5].u.nameHash = 0xA0AB1041;

        pass.args[6].type = MTL_ARG_CODE_VERTEX_CONST;
        pass.args[6].dest = 21;
        pass.args[6].u.codeConst.index = CONST_SRC_CODE_FOG;
        pass.args[6].u.codeConst.firstRow = 0;
        pass.args[6].u.codeConst.rowCount = 1;

        pass.args[7].type = MTL_ARG_CODE_PIXEL_SAMPLER;
        pass.args[7].dest = 4;
        pass.args[7].u.codeSampler = TEXTURE_SRC_CODE_MODEL_LIGHTING;

        pass.args[8].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[8].dest = 0;
        pass.args[8].u.codeConst.index = CONST_SRC_CODE_FOG_COLOR;
        pass.args[8].u.codeConst.firstRow = 0;
        pass.args[8].u.codeConst.rowCount = 1;

        pass.args[9].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[9].dest = 5;
        pass.args[9].u.codeConst.index = CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE;
        pass.args[9].u.codeConst.firstRow = 0;
        pass.args[9].u.codeConst.rowCount = 1;

        pass.args[10].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[10].dest = 6;
        pass.args[10].u.codeConst.index = CONST_SRC_CODE_LIGHT_POSITION;
        pass.args[10].u.codeConst.firstRow = 0;
        pass.args[10].u.codeConst.rowCount = 1;

        pass.args[11].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[11].dest = 7;
        pass.args[11].u.codeConst.index = CONST_SRC_CODE_LIGHT_DIFFUSE;
        pass.args[11].u.codeConst.firstRow = 0;
        pass.args[11].u.codeConst.rowCount = 1;

        pass.args[12].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[12].dest = 8;
        pass.args[12].u.codeConst.index = CONST_SRC_CODE_LIGHT_SPOTDIR;
        pass.args[12].u.codeConst.firstRow = 0;
        pass.args[12].u.codeConst.rowCount = 1;

        pass.args[13].type = MTL_ARG_CODE_PIXEL_CONST;
        pass.args[13].dest = 9;
        pass.args[13].u.codeConst.index = CONST_SRC_CODE_LIGHT_SPOTFACTORS;
        pass.args[13].u.codeConst.firstRow = 0;
        pass.args[13].u.codeConst.rowCount = 1;

        return technique;
    }

    MaterialTechniqueSet* GivenTechset(Zone& zone, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = "example_techset";
        techset->worldVertFormat = MTL_WORLDVERT_TEX_4_NRM_2;

        techset->techniques[TECHNIQUE_DEPTH_PREPASS] = GivenDepthPrepassTechnique(memory);
        techset->techniques[TECHNIQUE_LIT_SPOT] = GivenLitSpotTechnique(memory);

        zone.m_pools.AddAsset(std::make_unique<XAssetInfo<MaterialTechniqueSet>>(ASSET_TYPE_TECHNIQUE_SET, techset->name, techset));
        return techset;
    }
} // namespace

TEST_CASE("TechsetDumperIW3", "[iw3][techset][dumper]")
{
    Zone zone("MockZone", 0, GameId::IW3, GamePlatform::PC);
    zone.Register();

    MemoryManager memory;
    MockSearchPath mockObjPath;
    MockOutputPath mockOutput;
    AssetDumpingContext context(zone, "", mockOutput, mockObjPath, std::nullopt);

    GivenTechset(zone, memory);

    techset::DumperIW3 dumper(true);

    SECTION("Can dump techset")
    {
        std::string expected(R"TECHSET(
"depth prepass":
  example_zprepass;

"lit spot":
  example_lit_spot;
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
{
  stateMap "passthrough"; // TODO

  vertexShader 3.0 "advanced.hlsl"
  {
    // Omitted due to matching accessors: worldMatrix = constant.worldMatrix;
    // Omitted due to matching accessors: baseLightingCoords = constant.baseLightingCoords;
    // Omitted due to matching accessors: viewProjectionMatrix = constant.viewProjectionMatrix;
    // Omitted due to matching accessors: fogConsts = constant.fogConsts;
  }

  pixelShader 3.0 "advanced.hlsl"
  {
    // Omitted due to matching accessors: attenuationSampler = sampler.attenuationSampler;
    normalMapSampler = material.normalMap;
    colorMapSampler = material.colorMap;
    // Omitted due to matching accessors: modelLightingSampler = sampler.modelLightingSampler;
    // Omitted due to matching accessors: fogColor = constant.fogColor;
    // Omitted due to matching accessors: lightingLookupScale = constant.lightingLookupScale;
    // Omitted due to matching accessors: lightPosition = constant.lightPosition;
    // Omitted due to matching accessors: lightDiffuse = constant.lightDiffuse;
    // Omitted due to matching accessors: lightSpotDir = constant.lightSpotDir;
    // Omitted due to matching accessors: lightSpotFactors = constant.lightSpotFactors;
  }

  vertex.position = code.position;
  vertex.color[0] = code.color;
  vertex.texcoord[0] = code.texcoord[0];
  vertex.normal = code.normal;
  vertex.texcoord[2] = code.tangent;
}
)TECHNIQUE");
        dumper.Dump(context);

        const auto* file = mockOutput.GetMockedFile("techniques/example_lit_spot.tech");
        REQUIRE(file);
        REQUIRE(Trimmed(file->AsString()) == Trimmed(expected));
    }
}
