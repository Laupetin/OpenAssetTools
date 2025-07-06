#include "Game/T5/Material/DumperMaterialT5.h"

#include "Asset/AssetRegistration.h"
#include "Game/T5/CommonT5.h"
#include "Game/T5/GameT5.h"
#include "NormalizedJson.h"
#include "Pool/AssetPoolDynamic.h"
#include "SearchPath/MockOutputPath.h"
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
    GfxImage* GivenImage(const std::string& name, MemoryManager& memory)
    {
        auto* image = memory.Alloc<GfxImage>();
        image->name = memory.Dup(name.c_str());

        return image;
    }

    MaterialTechniqueSet* GivenTechset(const std::string& name, MemoryManager& memory)
    {
        auto* techset = memory.Alloc<MaterialTechniqueSet>();
        techset->name = memory.Dup(name.c_str());

        return techset;
    }

    void GivenMaterial(const std::string& name, AssetPool<Material>& pool, MemoryManager& memory)
    {
        auto* material = memory.Alloc<Material>();
        material->info.name = memory.Dup(name.c_str());
        material->info.gameFlags = 0x50;
        material->info.sortKey = 4;
        material->info.textureAtlasRowCount = 1;
        material->info.textureAtlasColumnCount = 1;
        material->info.surfaceTypeBits = 0x2;
        material->info.layeredSurfaceTypes = 0x20000002;

        constexpr int8_t stateBitsEntry[] = {0,  1,  2,  3,  1,  -1, -1, -1, -1, -1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                                             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                             -1, -1, -1, -1, -1, -1, -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  -1, 1,  1,  1,  1,  1,
                                             1,  1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                             -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4,  4,  4,  4,  4,  4,  -1, -1, -1, -1, 5,  -1, 1,  1,  -1};
        std::memcpy(material->stateBitsEntry, stateBitsEntry, sizeof(material->stateBitsEntry));

        material->cameraRegion = CAMERA_REGION_LIT;
        material->stateFlags = 121;
        material->maxStreamedMips = 0;
        material->techniqueSet = GivenTechset("mc_l_sm_r0c0n0s0", memory);

        material->textureCount = 3;
        material->textureTable = memory.Alloc<MaterialTextureDef>(3);

        auto& textureDef0 = material->textureTable[0];
        textureDef0.u.image = GivenImage("~$white-rgb&~-rjun_art_brick_~a05e7627", memory);
        textureDef0.nameHash = 0x34ecccb3;
        textureDef0.nameStart = 's';
        textureDef0.nameEnd = 'p';
        textureDef0.samplerState.filter = TEXTURE_FILTER_ANISO2X;
        textureDef0.samplerState.mipMap = SAMPLER_MIPMAP_ENUM_NEAREST;
        textureDef0.samplerState.clampU = 0;
        textureDef0.samplerState.clampV = 0;
        textureDef0.samplerState.clampW = 0;
        textureDef0.semantic = TS_SPECULAR_MAP;
        textureDef0.isMatureContent = false;

        auto& textureDef1 = material->textureTable[1];
        textureDef1.u.image = GivenImage("jun_art_brick_foundation_n", memory);
        textureDef1.nameHash = 0x59d30d0f;
        textureDef1.nameStart = 'n';
        textureDef1.nameEnd = 'p';
        textureDef1.samplerState.filter = TEXTURE_FILTER_ANISO2X;
        textureDef1.samplerState.mipMap = SAMPLER_MIPMAP_ENUM_NEAREST;
        textureDef1.samplerState.clampU = 0;
        textureDef1.samplerState.clampV = 0;
        textureDef1.samplerState.clampW = 0;
        textureDef1.semantic = TS_NORMAL_MAP;
        textureDef1.isMatureContent = false;

        auto& textureDef2 = material->textureTable[2];
        textureDef2.u.image = GivenImage("~-gjun_art_brick_foundation_c", memory);
        textureDef2.nameHash = 0xa0ab1041;
        textureDef2.nameStart = 'c';
        textureDef2.nameEnd = 'p';
        textureDef2.samplerState.filter = TEXTURE_FILTER_ANISO2X;
        textureDef2.samplerState.mipMap = SAMPLER_MIPMAP_ENUM_NEAREST;
        textureDef2.samplerState.clampU = 0;
        textureDef2.samplerState.clampV = 0;
        textureDef2.samplerState.clampW = 0;
        textureDef2.semantic = TS_COLOR_MAP;
        textureDef2.isMatureContent = false;

        material->constantCount = 2;
        material->constantTable = memory.Alloc<MaterialConstantDef>(2);

        auto& constantDef0 = material->constantTable[0];
        constantDef0.nameHash = 0x3d9994dc;
        strncpy(constantDef0.name, "envMapParms", std::extent_v<decltype(MaterialConstantDef::name)>);
        constantDef0.literal.x = 1.0f;
        constantDef0.literal.y = 1.0f;
        constantDef0.literal.z = 1.0f;
        constantDef0.literal.w = 0.5f;

        auto& constantDef1 = material->constantTable[1];
        constantDef1.nameHash = 0xb4de4d82;
        strncpy(constantDef1.name, "dynamicFolia", std::extent_v<decltype(MaterialConstantDef::name)>);
        constantDef1.literal.x = 0.0f;
        constantDef1.literal.y = 1.0f;
        constantDef1.literal.z = 0.0f;
        constantDef1.literal.w = 1.0f;

        material->stateBitsCount = 6;
        material->stateBitsTable = memory.Alloc<GfxStateBits>(6);

        auto& stateBits0 = material->stateBitsTable[0];
        stateBits0.loadBits.structured.srcBlendRgb = GFXS_BLEND_ONE;
        stateBits0.loadBits.structured.dstBlendRgb = GFXS_BLEND_ZERO;
        stateBits0.loadBits.structured.blendOpRgb = GFXS_BLENDOP_DISABLED;
        stateBits0.loadBits.structured.alphaTestDisabled = 1;
        stateBits0.loadBits.structured.alphaTest = 0;
        stateBits0.loadBits.structured.cullFace = GFXS_CULL_BACK;
        stateBits0.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits0.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits0.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits0.loadBits.structured.colorWriteRgb = 0;
        stateBits0.loadBits.structured.colorWriteAlpha = 0;
        stateBits0.loadBits.structured.polymodeLine = 0;
        stateBits0.loadBits.structured.depthWrite = 1;
        stateBits0.loadBits.structured.depthTestDisabled = 0;
        stateBits0.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits0.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_0;
        stateBits0.loadBits.structured.stencilFrontEnabled = 1;
        stateBits0.loadBits.structured.stencilFrontPass = GFXS_STENCILOP_KEEP;
        stateBits0.loadBits.structured.stencilFrontFail = GFXS_STENCILOP_KEEP;
        stateBits0.loadBits.structured.stencilFrontZFail = GFXS_STENCILOP_KEEP;
        stateBits0.loadBits.structured.stencilFrontFunc = GFXS_STENCILFUNC_EQUAL;
        stateBits0.loadBits.structured.stencilBackEnabled = 0;
        stateBits0.loadBits.structured.stencilBackPass = 0;
        stateBits0.loadBits.structured.stencilBackFail = 0;
        stateBits0.loadBits.structured.stencilBackZFail = 0;
        stateBits0.loadBits.structured.stencilBackFunc = 0;

        auto& stateBits1 = material->stateBitsTable[1];
        stateBits1.loadBits.structured.srcBlendRgb = GFXS_BLEND_ONE;
        stateBits1.loadBits.structured.dstBlendRgb = GFXS_BLEND_ZERO;
        stateBits1.loadBits.structured.blendOpRgb = GFXS_BLENDOP_DISABLED;
        stateBits1.loadBits.structured.alphaTestDisabled = 1;
        stateBits1.loadBits.structured.alphaTest = 0;
        stateBits1.loadBits.structured.cullFace = GFXS_CULL_BACK;
        stateBits1.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits1.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits1.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits1.loadBits.structured.colorWriteRgb = 1;
        stateBits1.loadBits.structured.colorWriteAlpha = 1;
        stateBits1.loadBits.structured.polymodeLine = 0;
        stateBits1.loadBits.structured.depthWrite = 1;
        stateBits1.loadBits.structured.depthTestDisabled = 0;
        stateBits1.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits1.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_0;
        stateBits1.loadBits.structured.stencilFrontEnabled = 0;
        stateBits1.loadBits.structured.stencilFrontPass = 0;
        stateBits1.loadBits.structured.stencilFrontFail = 0;
        stateBits1.loadBits.structured.stencilFrontZFail = 0;
        stateBits1.loadBits.structured.stencilFrontFunc = 0;
        stateBits1.loadBits.structured.stencilBackEnabled = 0;
        stateBits1.loadBits.structured.stencilBackPass = 0;
        stateBits1.loadBits.structured.stencilBackFail = 0;
        stateBits1.loadBits.structured.stencilBackZFail = 0;
        stateBits1.loadBits.structured.stencilBackFunc = 0;

        auto& stateBits2 = material->stateBitsTable[2];
        stateBits2.loadBits.structured.srcBlendRgb = GFXS_BLEND_ONE;
        stateBits2.loadBits.structured.dstBlendRgb = GFXS_BLEND_ZERO;
        stateBits2.loadBits.structured.blendOpRgb = GFXS_BLENDOP_DISABLED;
        stateBits2.loadBits.structured.alphaTestDisabled = 1;
        stateBits2.loadBits.structured.alphaTest = 0;
        stateBits2.loadBits.structured.cullFace = GFXS_CULL_BACK;
        stateBits2.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits2.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits2.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits2.loadBits.structured.colorWriteRgb = 0;
        stateBits2.loadBits.structured.colorWriteAlpha = 0;
        stateBits2.loadBits.structured.polymodeLine = 0;
        stateBits2.loadBits.structured.depthWrite = 1;
        stateBits2.loadBits.structured.depthTestDisabled = 0;
        stateBits2.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits2.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_SHADOWMAP;
        stateBits2.loadBits.structured.stencilFrontEnabled = 0;
        stateBits2.loadBits.structured.stencilFrontPass = 0;
        stateBits2.loadBits.structured.stencilFrontFail = 0;
        stateBits2.loadBits.structured.stencilFrontZFail = 0;
        stateBits2.loadBits.structured.stencilFrontFunc = 0;
        stateBits2.loadBits.structured.stencilBackEnabled = 0;
        stateBits2.loadBits.structured.stencilBackPass = 0;
        stateBits2.loadBits.structured.stencilBackFail = 0;
        stateBits2.loadBits.structured.stencilBackZFail = 0;
        stateBits2.loadBits.structured.stencilBackFunc = 0;

        auto& stateBits3 = material->stateBitsTable[3];
        stateBits3.loadBits.structured.srcBlendRgb = GFXS_BLEND_ONE;
        stateBits3.loadBits.structured.dstBlendRgb = GFXS_BLEND_ZERO;
        stateBits3.loadBits.structured.blendOpRgb = GFXS_BLENDOP_DISABLED;
        stateBits3.loadBits.structured.alphaTestDisabled = 1;
        stateBits3.loadBits.structured.alphaTest = 0;
        stateBits3.loadBits.structured.cullFace = GFXS_CULL_NONE;
        stateBits3.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits3.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits3.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits3.loadBits.structured.colorWriteRgb = 1;
        stateBits3.loadBits.structured.colorWriteAlpha = 1;
        stateBits3.loadBits.structured.polymodeLine = 0;
        stateBits3.loadBits.structured.depthWrite = 1;
        stateBits3.loadBits.structured.depthTestDisabled = 0;
        stateBits3.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits3.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_0;
        stateBits3.loadBits.structured.stencilFrontEnabled = 0;
        stateBits3.loadBits.structured.stencilFrontPass = 0;
        stateBits3.loadBits.structured.stencilFrontFail = 0;
        stateBits3.loadBits.structured.stencilFrontZFail = 0;
        stateBits3.loadBits.structured.stencilFrontFunc = 0;
        stateBits3.loadBits.structured.stencilBackEnabled = 0;
        stateBits3.loadBits.structured.stencilBackPass = 0;
        stateBits3.loadBits.structured.stencilBackFail = 0;
        stateBits3.loadBits.structured.stencilBackZFail = 0;
        stateBits3.loadBits.structured.stencilBackFunc = 0;

        auto& stateBits4 = material->stateBitsTable[4];
        stateBits4.loadBits.structured.srcBlendRgb = GFXS_BLEND_INVDESTALPHA;
        stateBits4.loadBits.structured.dstBlendRgb = GFXS_BLEND_ONE;
        stateBits4.loadBits.structured.blendOpRgb = GFXS_BLENDOP_ADD;
        stateBits4.loadBits.structured.alphaTestDisabled = 1;
        stateBits4.loadBits.structured.alphaTest = 0;
        stateBits4.loadBits.structured.cullFace = GFXS_CULL_BACK;
        stateBits4.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits4.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits4.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits4.loadBits.structured.colorWriteRgb = 1;
        stateBits4.loadBits.structured.colorWriteAlpha = 1;
        stateBits4.loadBits.structured.polymodeLine = 0;
        stateBits4.loadBits.structured.depthWrite = 0;
        stateBits4.loadBits.structured.depthTestDisabled = 0;
        stateBits4.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits4.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_0;
        stateBits4.loadBits.structured.stencilFrontEnabled = 0;
        stateBits4.loadBits.structured.stencilFrontPass = 0;
        stateBits4.loadBits.structured.stencilFrontFail = 0;
        stateBits4.loadBits.structured.stencilFrontZFail = 0;
        stateBits4.loadBits.structured.stencilFrontFunc = 0;
        stateBits4.loadBits.structured.stencilBackEnabled = 0;
        stateBits4.loadBits.structured.stencilBackPass = 0;
        stateBits4.loadBits.structured.stencilBackFail = 0;
        stateBits4.loadBits.structured.stencilBackZFail = 0;
        stateBits4.loadBits.structured.stencilBackFunc = 0;

        auto& stateBits5 = material->stateBitsTable[5];
        stateBits5.loadBits.structured.srcBlendRgb = GFXS_BLEND_ONE;
        stateBits5.loadBits.structured.dstBlendRgb = GFXS_BLEND_ZERO;
        stateBits5.loadBits.structured.blendOpRgb = GFXS_BLENDOP_DISABLED;
        stateBits5.loadBits.structured.alphaTestDisabled = 1;
        stateBits5.loadBits.structured.alphaTest = 0;
        stateBits5.loadBits.structured.cullFace = GFXS_CULL_BACK;
        stateBits5.loadBits.structured.srcBlendAlpha = GFXS_BLEND_ONE;
        stateBits5.loadBits.structured.dstBlendAlpha = GFXS_BLEND_ZERO;
        stateBits5.loadBits.structured.blendOpAlpha = GFXS_BLENDOP_DISABLED;
        stateBits5.loadBits.structured.colorWriteRgb = 1;
        stateBits5.loadBits.structured.colorWriteAlpha = 0;
        stateBits5.loadBits.structured.polymodeLine = 1;
        stateBits5.loadBits.structured.depthWrite = 0;
        stateBits5.loadBits.structured.depthTestDisabled = 0;
        stateBits5.loadBits.structured.depthTest = GFXS_DEPTHTEST_LESSEQUAL;
        stateBits5.loadBits.structured.polygonOffset = GFXS_POLYGON_OFFSET_2;
        stateBits5.loadBits.structured.stencilFrontEnabled = 0;
        stateBits5.loadBits.structured.stencilFrontPass = 0;
        stateBits5.loadBits.structured.stencilFrontFail = 0;
        stateBits5.loadBits.structured.stencilFrontZFail = 0;
        stateBits5.loadBits.structured.stencilFrontFunc = 0;
        stateBits5.loadBits.structured.stencilBackEnabled = 0;
        stateBits5.loadBits.structured.stencilBackPass = 0;
        stateBits5.loadBits.structured.stencilBackFail = 0;
        stateBits5.loadBits.structured.stencilBackZFail = 0;
        stateBits5.loadBits.structured.stencilBackFunc = 0;

        pool.AddAsset(std::make_unique<XAssetInfo<Material>>(ASSET_TYPE_MATERIAL, name, material));
    }

    TEST_CASE("DumperMaterial(T5): Can dump material", "[t5][material][assetdumper]")
    {
        std::string expected(R"MATERIAL(
{
    "$schema": "https://openassettools.dev/schema/material.v1.json",
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
    "maxStreamedMips": 0,
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
        MockSearchPath mockObjPath;
        MockOutputPath mockOutput;
        AssetDumpingContext context(zone, "", mockOutput, mockObjPath);

        AssetPoolDynamic<Material> materialPool(0);

        GivenMaterial("mc/ch_rubble01", materialPool, memory);

        AssetDumperMaterial dumper;
        dumper.DumpPool(context, &materialPool);

        const auto* file = mockOutput.GetMockedFile("materials/mc/ch_rubble01.json");
        REQUIRE(file);
        REQUIRE(JsonNormalized(file->AsString()) == JsonNormalized(expected));
    }
} // namespace
