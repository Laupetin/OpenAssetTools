#include "Game/T5/Material/LoaderMaterialT5.h"

#include "Game/T5/CommonT5.h"
#include "Game/T5/GameT5.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T5;
using namespace Catch;
using namespace std::literals;

namespace
{
    void GivenImage(const std::string& name, AssetCreationContext& context, MemoryManager& memory)
    {
        auto* image = memory.Alloc<GfxImage>();
        image->name = memory.Dup(name.c_str());

        AssetRegistration<AssetImage> registration(name);
        registration.SetAsset(image);
        context.AddAsset(std::move(registration));
    }

    void GivenTechset(const std::string& name, AssetCreationContext& context, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = memory.Dup(name.c_str());

        AssetRegistration<AssetTechniqueSet> registration(name);
        registration.SetAsset(techset);
        context.AddAsset(std::move(registration));
    }

    TEST_CASE("LoaderMaterial(T5): Can parse material", "[t5][material][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("materials/mc/jun_art_brick_foundation.json",
                               R"MATERIAL(
{
    "_game": "t5",
    "_type": "material",
    "_version": 1,
    "cameraRegion": "lit",
    "constants": [
        {
            "literal": [
                1.0,
                1.0,
                1.0,
                0.5
            ],
            "name": "envMapParms"
        },
        {
            "literal": [
                0.0,
                1.0,
                0.0,
                1.0
            ],
            "nameFragment": "dynamicFolia",
            "nameHash": 3034467714
        }
    ],
    "gameFlags": [
        "10",
        "CASTS_SHADOW"
    ],
    "layeredSurfaceTypes": 536870914,
    "sortKey": 4,
    "stateBits": [
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "disabled",
            "colorWriteAlpha": false,
            "colorWriteRgb": false,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": true,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "zero",
            "polygonOffset": "offset0",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one",
            "stencilFront": {
                "fail": "keep",
                "func": "equal",
                "pass": "keep",
                "zfail": "keep"
            }
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "disabled",
            "colorWriteAlpha": true,
            "colorWriteRgb": true,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": true,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "zero",
            "polygonOffset": "offset0",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one"
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "disabled",
            "colorWriteAlpha": false,
            "colorWriteRgb": false,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": true,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "zero",
            "polygonOffset": "offsetShadowmap",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one"
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "disabled",
            "colorWriteAlpha": true,
            "colorWriteRgb": true,
            "cullFace": "none",
            "depthTest": "less_equal",
            "depthWrite": true,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "zero",
            "polygonOffset": "offset0",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one"
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "add",
            "colorWriteAlpha": true,
            "colorWriteRgb": true,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": false,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "one",
            "polygonOffset": "offset0",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "invdestalpha"
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "disabled",
            "colorWriteAlpha": false,
            "colorWriteRgb": true,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": false,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "zero",
            "polygonOffset": "offset2",
            "polymodeLine": true,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one"
        }
    ],
    "stateBitsEntry": [
        0,
        1,
        2,
        3,
        1,
        -1,
        -1,
        -1,
        -1,
        -1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        -1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        4,
        4,
        4,
        4,
        4,
        4,
        -1,
        -1,
        -1,
        -1,
        5,
        -1,
        1,
        1,
        -1
    ],
    "stateFlags": 121,
    "surfaceTypeBits": 2,
    "techniqueSet": "mc_l_sm_r0c0n0s0",
    "textureAtlas": {
        "columns": 1,
        "rows": 1
    },
    "textures": [
        {
            "image": "~$white-rgb&~-rjun_art_brick_~a05e7627",
            "isMatureContent": false,
            "name": "specularMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso2x",
                "mipMap": "nearest"
            },
            "semantic": "specularMap"
        },
        {
            "image": "jun_art_brick_foundation_n",
            "isMatureContent": false,
            "name": "normalMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso2x",
                "mipMap": "nearest"
            },
            "semantic": "normalMap"
        },
        {
            "image": "~-gjun_art_brick_foundation_c",
            "isMatureContent": false,
            "name": "colorMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso2x",
                "mipMap": "nearest"
            },
            "semantic": "colorMap"
        }
    ]
})MATERIAL");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T5));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        GivenImage("~$white-rgb&~-rjun_art_brick_~a05e7627", context, memory);
        GivenImage("jun_art_brick_foundation_n", context, memory);
        GivenImage("~-gjun_art_brick_foundation_c", context, memory);
        GivenTechset("mc_l_sm_r0c0n0s0", context, memory);

        auto loader = CreateMaterialLoader(memory, searchPath);
        auto result = loader->CreateAsset("mc/jun_art_brick_foundation", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<Material>*>(result.GetAssetInfo());
        const auto* material = assetInfo->Asset();

        REQUIRE(material->info.name == "mc/jun_art_brick_foundation"s);
        REQUIRE(material->info.gameFlags == 0x50);
        REQUIRE(material->info.sortKey == 4);
        REQUIRE(material->info.textureAtlasRowCount == 1);
        REQUIRE(material->info.textureAtlasColumnCount == 1);
        REQUIRE(material->info.surfaceTypeBits == 0x2);
        REQUIRE(material->info.layeredSurfaceTypes == 0x20000002);

        constexpr int8_t expectedStateBitsEntry[]{0,  1,  2,  3,  1,  -1, -1, -1, -1, -1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                                                  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                  -1, -1, -1, -1, -1, -1, -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  1,  1,  1,  1,
                                                  1,  1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4,  4,  4,  4,  4,  4,  -1, -1, -1, -1, 5,  -1, 1,  1,  -1};
        REQUIRE(std::memcmp(material->stateBitsEntry, expectedStateBitsEntry, sizeof(expectedStateBitsEntry)) == 0);

        REQUIRE(material->stateFlags == 121);
        REQUIRE(material->cameraRegion == CAMERA_REGION_LIT);

        REQUIRE(material->techniqueSet != nullptr);
        REQUIRE(material->techniqueSet->name != nullptr);
        REQUIRE(material->techniqueSet->name == "mc_l_sm_r0c0n0s0"s);

        REQUIRE(material->textureCount == 3);
        REQUIRE(material->textureTable);

        const auto& textureDef0 = material->textureTable[0];
        REQUIRE(textureDef0.nameHash == 0x34ecccb3);
        REQUIRE(textureDef0.nameStart == 's');
        REQUIRE(textureDef0.nameEnd == 'p');
        REQUIRE(textureDef0.samplerState.filter == TEXTURE_FILTER_ANISO2X);
        REQUIRE(textureDef0.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_NEAREST);
        REQUIRE(textureDef0.samplerState.clampU == 0);
        REQUIRE(textureDef0.samplerState.clampV == 0);
        REQUIRE(textureDef0.samplerState.clampW == 0);
        REQUIRE(textureDef0.semantic == TS_SPECULAR_MAP);
        REQUIRE(textureDef0.isMatureContent == false);
        REQUIRE(textureDef0.u.image);
        REQUIRE(textureDef0.u.image->name);
        REQUIRE(textureDef0.u.image->name == "~$white-rgb&~-rjun_art_brick_~a05e7627"s);

        const auto& textureDef1 = material->textureTable[1];
        REQUIRE(textureDef1.nameHash == 0x59d30d0f);
        REQUIRE(textureDef1.nameStart == 'n');
        REQUIRE(textureDef1.nameEnd == 'p');
        REQUIRE(textureDef1.samplerState.filter == TEXTURE_FILTER_ANISO2X);
        REQUIRE(textureDef1.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_NEAREST);
        REQUIRE(textureDef1.samplerState.clampU == 0);
        REQUIRE(textureDef1.samplerState.clampV == 0);
        REQUIRE(textureDef1.samplerState.clampW == 0);
        REQUIRE(textureDef1.semantic == TS_NORMAL_MAP);
        REQUIRE(textureDef1.isMatureContent == false);
        REQUIRE(textureDef1.u.image);
        REQUIRE(textureDef1.u.image->name);
        REQUIRE(textureDef1.u.image->name == "jun_art_brick_foundation_n"s);

        const auto& textureDef2 = material->textureTable[2];
        REQUIRE(textureDef2.nameHash == 0xa0ab1041);
        REQUIRE(textureDef2.nameStart == 'c');
        REQUIRE(textureDef2.nameEnd == 'p');
        REQUIRE(textureDef2.samplerState.filter == TEXTURE_FILTER_ANISO2X);
        REQUIRE(textureDef2.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_NEAREST);
        REQUIRE(textureDef2.samplerState.clampU == 0);
        REQUIRE(textureDef2.samplerState.clampV == 0);
        REQUIRE(textureDef2.samplerState.clampW == 0);
        REQUIRE(textureDef2.semantic == TS_COLOR_MAP);
        REQUIRE(textureDef2.isMatureContent == false);
        REQUIRE(textureDef2.u.image);
        REQUIRE(textureDef2.u.image->name);
        REQUIRE(textureDef2.u.image->name == "~-gjun_art_brick_foundation_c"s);

        REQUIRE(material->constantCount == 2);
        REQUIRE(material->constantTable);

        const auto& constantDef0 = material->constantTable[0];
        REQUIRE(constantDef0.nameHash == 0x3d9994dc);
        REQUIRE(strncmp(constantDef0.name, "envMapParms", std::extent_v<decltype(MaterialConstantDef::name)>) == 0);
        REQUIRE(constantDef0.literal.x == Approx(1.0f));
        REQUIRE(constantDef0.literal.y == Approx(1.0f));
        REQUIRE(constantDef0.literal.z == Approx(1.0f));
        REQUIRE(constantDef0.literal.w == Approx(0.5f));

        const auto& constantDef1 = material->constantTable[1];
        REQUIRE(constantDef1.nameHash == 0xb4de4d82);
        REQUIRE(strncmp(constantDef1.name, "dynamicFolia", std::extent_v<decltype(MaterialConstantDef::name)>) == 0);
        REQUIRE(constantDef1.literal.x == Approx(0.0f));
        REQUIRE(constantDef1.literal.y == Approx(1.0f));
        REQUIRE(constantDef1.literal.z == Approx(0.0f));
        REQUIRE(constantDef1.literal.w == Approx(1.0f));

        REQUIRE(material->stateBitsCount == 6);
        REQUIRE(material->stateBitsTable);

        const auto& stateBits0 = material->stateBitsTable[0];
        REQUIRE(stateBits0.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits0.loadBits.structured.dstBlendRgb == GFXS_BLEND_ZERO);
        REQUIRE(stateBits0.loadBits.structured.blendOpRgb == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits0.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits0.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits0.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits0.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits0.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits0.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits0.loadBits.structured.colorWriteRgb == 0);
        REQUIRE(stateBits0.loadBits.structured.colorWriteAlpha == 0);
        REQUIRE(stateBits0.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits0.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits0.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits0.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits0.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_0);
        REQUIRE(stateBits0.loadBits.structured.stencilFrontEnabled == 1);
        REQUIRE(stateBits0.loadBits.structured.stencilFrontPass == GFXS_STENCILOP_KEEP);
        REQUIRE(stateBits0.loadBits.structured.stencilFrontFail == GFXS_STENCILOP_KEEP);
        REQUIRE(stateBits0.loadBits.structured.stencilFrontZFail == GFXS_STENCILOP_KEEP);
        REQUIRE(stateBits0.loadBits.structured.stencilFrontFunc == GFXS_STENCILFUNC_EQUAL);
        REQUIRE(stateBits0.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits0.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits0.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits0.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits0.loadBits.structured.stencilBackFunc == 0);

        const auto& stateBits1 = material->stateBitsTable[1];
        REQUIRE(stateBits1.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits1.loadBits.structured.dstBlendRgb == GFXS_BLEND_ZERO);
        REQUIRE(stateBits1.loadBits.structured.blendOpRgb == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits1.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits1.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits1.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits1.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits1.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits1.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits1.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits1.loadBits.structured.colorWriteAlpha == 1);
        REQUIRE(stateBits1.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits1.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits1.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits1.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits1.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_0);
        REQUIRE(stateBits1.loadBits.structured.stencilFrontEnabled == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilFrontPass == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilFrontFail == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilFrontZFail == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilFrontFunc == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits1.loadBits.structured.stencilBackFunc == 0);

        const auto& stateBits2 = material->stateBitsTable[2];
        REQUIRE(stateBits2.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits2.loadBits.structured.dstBlendRgb == GFXS_BLEND_ZERO);
        REQUIRE(stateBits2.loadBits.structured.blendOpRgb == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits2.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits2.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits2.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits2.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits2.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits2.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits2.loadBits.structured.colorWriteRgb == 0);
        REQUIRE(stateBits2.loadBits.structured.colorWriteAlpha == 0);
        REQUIRE(stateBits2.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits2.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits2.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits2.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits2.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_SHADOWMAP);
        REQUIRE(stateBits2.loadBits.structured.stencilFrontEnabled == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilFrontPass == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilFrontFail == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilFrontZFail == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilFrontFunc == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits2.loadBits.structured.stencilBackFunc == 0);

        const auto& stateBits3 = material->stateBitsTable[3];
        REQUIRE(stateBits3.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits3.loadBits.structured.dstBlendRgb == GFXS_BLEND_ZERO);
        REQUIRE(stateBits3.loadBits.structured.blendOpRgb == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits3.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits3.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits3.loadBits.structured.cullFace == GFXS_CULL_NONE);
        REQUIRE(stateBits3.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits3.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits3.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits3.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits3.loadBits.structured.colorWriteAlpha == 1);
        REQUIRE(stateBits3.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits3.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits3.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits3.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits3.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_0);
        REQUIRE(stateBits3.loadBits.structured.stencilFrontEnabled == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilFrontPass == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilFrontFail == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilFrontZFail == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilFrontFunc == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits3.loadBits.structured.stencilBackFunc == 0);

        const auto& stateBits4 = material->stateBitsTable[4];
        REQUIRE(stateBits4.loadBits.structured.srcBlendRgb == GFXS_BLEND_INVDESTALPHA);
        REQUIRE(stateBits4.loadBits.structured.dstBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits4.loadBits.structured.blendOpRgb == GFXS_BLENDOP_ADD);
        REQUIRE(stateBits4.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits4.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits4.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits4.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits4.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits4.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits4.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits4.loadBits.structured.colorWriteAlpha == 1);
        REQUIRE(stateBits4.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits4.loadBits.structured.depthWrite == 0);
        REQUIRE(stateBits4.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits4.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits4.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_0);
        REQUIRE(stateBits4.loadBits.structured.stencilFrontEnabled == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilFrontPass == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilFrontFail == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilFrontZFail == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilFrontFunc == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits4.loadBits.structured.stencilBackFunc == 0);

        const auto& stateBits5 = material->stateBitsTable[5];
        REQUIRE(stateBits5.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
        REQUIRE(stateBits5.loadBits.structured.dstBlendRgb == GFXS_BLEND_ZERO);
        REQUIRE(stateBits5.loadBits.structured.blendOpRgb == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits5.loadBits.structured.alphaTestDisabled == 1);
        REQUIRE(stateBits5.loadBits.structured.alphaTest == 0);
        REQUIRE(stateBits5.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits5.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits5.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits5.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits5.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits5.loadBits.structured.colorWriteAlpha == 0);
        REQUIRE(stateBits5.loadBits.structured.polymodeLine == 1);
        REQUIRE(stateBits5.loadBits.structured.depthWrite == 0);
        REQUIRE(stateBits5.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits5.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits5.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_2);
        REQUIRE(stateBits5.loadBits.structured.stencilFrontEnabled == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilFrontPass == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilFrontFail == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilFrontZFail == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilFrontFunc == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilBackEnabled == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilBackPass == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilBackFail == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilBackZFail == 0);
        REQUIRE(stateBits5.loadBits.structured.stencilBackFunc == 0);
    }
} // namespace
