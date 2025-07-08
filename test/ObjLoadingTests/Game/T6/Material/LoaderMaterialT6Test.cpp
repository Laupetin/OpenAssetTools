#include "Game/T6/Material/LoaderMaterialT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameT6.h"
#include "SearchPath/MockSearchPath.h"
#include "Utils/MemoryManager.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace T6;
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

    TEST_CASE("LoaderMaterial(T6): Can parse material", "[t6][material][assetloader]")
    {
        MockSearchPath searchPath;
        searchPath.AddFileData("materials/wpc/metal_ac_duct.json",
                               R"MATERIAL(
{
    "_game": "t6",
    "_type": "material",
    "_version": 1,
    "cameraRegion": "litOpaque",
    "constants": [
        {
            "literal": [
                1.0,
                1.0,
                1.0,
                1.0
            ],
            "name": "occlusionAmount"
        }
    ],
    "contents": 1,
    "gameFlags": [
        "2",
        "10",
        "CASTS_SHADOW"
    ],
    "layeredSurfaceTypes": 536870925,
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
        },
        {
            "alphaTest": "disabled",
            "blendOpAlpha": "disabled",
            "blendOpRgb": "add",
            "colorWriteAlpha": true,
            "colorWriteRgb": true,
            "cullFace": "back",
            "depthTest": "less_equal",
            "depthWrite": true,
            "dstBlendAlpha": "zero",
            "dstBlendRgb": "one",
            "polygonOffset": "offset1",
            "polymodeLine": false,
            "srcBlendAlpha": "one",
            "srcBlendRgb": "one"
        }
    ],
    "stateBitsEntry": [
        0,
        1,
        2,
        -1,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        2,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        3,
        -1,
        2,
        4
    ],
    "stateFlags": 121,
    "surfaceFlags": 13631488,
    "surfaceTypeBits": 4096,
    "techniqueSet": "wpc_lit_sm_r0c0n0s0_1zzj1138",
    "textureAtlas": {
        "columns": 1,
        "rows": 1
    },
    "textures": [
        {
            "image": "~~-gmetal_ac_duct_s-rgb&~-rme~0a5a2e6f",
            "isMatureContent": false,
            "name": "specularMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso4x",
                "mipMap": "linear"
            },
            "semantic": "specularMap"
        },
        {
            "image": "metal_ac_duct_n",
            "isMatureContent": false,
            "name": "normalMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso4x",
                "mipMap": "linear"
            },
            "semantic": "normalMap"
        },
        {
            "image": "~-gmetal_ac_duct_c",
            "isMatureContent": false,
            "name": "colorMap",
            "samplerState": {
                "clampU": false,
                "clampV": false,
                "clampW": false,
                "filter": "aniso4x",
                "mipMap": "linear"
            },
            "semantic": "colorMap"
        }
    ]
})MATERIAL");

        Zone zone("MockZone", 0, IGame::GetGameById(GameId::T6));

        MemoryManager memory;
        AssetCreatorCollection creatorCollection(zone);
        IgnoredAssetLookup ignoredAssetLookup;
        AssetCreationContext context(zone, &creatorCollection, &ignoredAssetLookup);

        GivenImage("~~-gmetal_ac_duct_s-rgb&~-rme~0a5a2e6f", context, memory);
        GivenImage("metal_ac_duct_n", context, memory);
        GivenImage("~-gmetal_ac_duct_c", context, memory);
        GivenTechset("wpc_lit_sm_r0c0n0s0_1zzj1138", context, memory);

        auto loader = CreateMaterialLoader(memory, searchPath);
        auto result = loader->CreateAsset("wpc/metal_ac_duct", context);
        REQUIRE(result.HasBeenSuccessful());

        const auto* assetInfo = reinterpret_cast<XAssetInfo<Material>*>(result.GetAssetInfo());
        const auto* material = assetInfo->Asset();

        REQUIRE(material->info.name == "wpc/metal_ac_duct"s);
        REQUIRE(material->info.gameFlags == 0x52);
        REQUIRE(material->info.sortKey == 4);
        REQUIRE(material->info.textureAtlasRowCount == 1);
        REQUIRE(material->info.textureAtlasColumnCount == 1);
        REQUIRE(material->info.surfaceTypeBits == 4096);
        REQUIRE(material->info.layeredSurfaceTypes == 0x2000000Du);
        REQUIRE(material->info.surfaceFlags == 0xD00000u);
        REQUIRE(material->info.contents == 1);

        constexpr int8_t expectedStateBitsEntry[]{0, 1, 2, -1, 2, 2, 2, 2, 2,  2,  2,  2,  2,  2,  2, 2,  2, 2,
                                                  2, 2, 2, 2,  2, 2, 2, 2, -1, -1, -1, -1, -1, -1, 3, -1, 2, 4};
        REQUIRE(std::memcmp(material->stateBitsEntry, expectedStateBitsEntry, sizeof(expectedStateBitsEntry)) == 0);

        REQUIRE(material->stateFlags == 121);
        REQUIRE(material->cameraRegion == CAMERA_REGION_LIT_OPAQUE);
        REQUIRE(material->probeMipBits == 0);

        REQUIRE(material->techniqueSet != nullptr);
        REQUIRE(material->techniqueSet->name != nullptr);
        REQUIRE(material->techniqueSet->name == "wpc_lit_sm_r0c0n0s0_1zzj1138"s);

        REQUIRE(material->textureCount == 3);
        REQUIRE(material->textureTable);

        const auto& textureDef0 = material->textureTable[0];
        REQUIRE(textureDef0.nameHash == 0x34ecccb3);
        REQUIRE(textureDef0.nameStart == 's');
        REQUIRE(textureDef0.nameEnd == 'p');
        REQUIRE(textureDef0.samplerState.filter == TEXTURE_FILTER_ANISO4X);
        REQUIRE(textureDef0.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_LINEAR);
        REQUIRE(textureDef0.samplerState.clampU == 0);
        REQUIRE(textureDef0.samplerState.clampV == 0);
        REQUIRE(textureDef0.samplerState.clampW == 0);
        REQUIRE(textureDef0.semantic == TS_SPECULAR_MAP);
        REQUIRE(textureDef0.isMatureContent == false);
        REQUIRE(textureDef0.image);
        REQUIRE(textureDef0.image->name);
        REQUIRE(textureDef0.image->name == "~~-gmetal_ac_duct_s-rgb&~-rme~0a5a2e6f"s);

        const auto& textureDef1 = material->textureTable[1];
        REQUIRE(textureDef1.nameHash == 0x59d30d0f);
        REQUIRE(textureDef1.nameStart == 'n');
        REQUIRE(textureDef1.nameEnd == 'p');
        REQUIRE(textureDef1.samplerState.filter == TEXTURE_FILTER_ANISO4X);
        REQUIRE(textureDef1.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_LINEAR);
        REQUIRE(textureDef1.samplerState.clampU == 0);
        REQUIRE(textureDef1.samplerState.clampV == 0);
        REQUIRE(textureDef1.samplerState.clampW == 0);
        REQUIRE(textureDef1.semantic == TS_NORMAL_MAP);
        REQUIRE(textureDef1.isMatureContent == false);
        REQUIRE(textureDef1.image);
        REQUIRE(textureDef1.image->name);
        REQUIRE(textureDef1.image->name == "metal_ac_duct_n"s);

        const auto& textureDef2 = material->textureTable[2];
        REQUIRE(textureDef2.nameHash == 0xa0ab1041);
        REQUIRE(textureDef2.nameStart == 'c');
        REQUIRE(textureDef2.nameEnd == 'p');
        REQUIRE(textureDef2.samplerState.filter == TEXTURE_FILTER_ANISO4X);
        REQUIRE(textureDef2.samplerState.mipMap == SAMPLER_MIPMAP_ENUM_LINEAR);
        REQUIRE(textureDef2.samplerState.clampU == 0);
        REQUIRE(textureDef2.samplerState.clampV == 0);
        REQUIRE(textureDef2.samplerState.clampW == 0);
        REQUIRE(textureDef2.semantic == TS_COLOR_MAP);
        REQUIRE(textureDef2.isMatureContent == false);
        REQUIRE(textureDef2.image);
        REQUIRE(textureDef2.image->name);
        REQUIRE(textureDef2.image->name == "~-gmetal_ac_duct_c"s);

        REQUIRE(material->constantCount == 1);
        REQUIRE(material->constantTable);

        const auto& constantDef0 = material->constantTable[0];
        REQUIRE(constantDef0.nameHash == 0x9027e5c1);
        REQUIRE(strncmp(constantDef0.name, "occlusionAmount", std::extent_v<decltype(MaterialConstantDef::name)>) == 0);
        REQUIRE(constantDef0.literal.x == Approx(1.0f));
        REQUIRE(constantDef0.literal.y == Approx(1.0f));
        REQUIRE(constantDef0.literal.z == Approx(1.0f));
        REQUIRE(constantDef0.literal.w == Approx(1.0f));

        REQUIRE(material->stateBitsCount == 5);
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
        REQUIRE(stateBits1.loadBits.structured.colorWriteRgb == 0);
        REQUIRE(stateBits1.loadBits.structured.colorWriteAlpha == 0);
        REQUIRE(stateBits1.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits1.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits1.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits1.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits1.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_SHADOWMAP);
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
        REQUIRE(stateBits2.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits2.loadBits.structured.colorWriteAlpha == 1);
        REQUIRE(stateBits2.loadBits.structured.polymodeLine == 0);
        REQUIRE(stateBits2.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits2.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits2.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits2.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_0);
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
        REQUIRE(stateBits3.loadBits.structured.cullFace == GFXS_CULL_BACK);
        REQUIRE(stateBits3.loadBits.structured.srcBlendAlpha == GFXS_BLEND_ONE);
        REQUIRE(stateBits3.loadBits.structured.dstBlendAlpha == GFXS_BLEND_ZERO);
        REQUIRE(stateBits3.loadBits.structured.blendOpAlpha == GFXS_BLENDOP_DISABLED);
        REQUIRE(stateBits3.loadBits.structured.colorWriteRgb == 1);
        REQUIRE(stateBits3.loadBits.structured.colorWriteAlpha == 0);
        REQUIRE(stateBits3.loadBits.structured.polymodeLine == 1);
        REQUIRE(stateBits3.loadBits.structured.depthWrite == 0);
        REQUIRE(stateBits3.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits3.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits3.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_2);
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
        REQUIRE(stateBits4.loadBits.structured.srcBlendRgb == GFXS_BLEND_ONE);
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
        REQUIRE(stateBits4.loadBits.structured.depthWrite == 1);
        REQUIRE(stateBits4.loadBits.structured.depthTestDisabled == 0);
        REQUIRE(stateBits4.loadBits.structured.depthTest == GFXS_DEPTHTEST_LESSEQUAL);
        REQUIRE(stateBits4.loadBits.structured.polygonOffset == GFXS_POLYGON_OFFSET_1);
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

        REQUIRE(material->thermalMaterial == nullptr);
    }
} // namespace
