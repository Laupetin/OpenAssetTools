#pragma once

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"

namespace T6
{
    inline const char* textureSemanticNames[]{
        "2D",           "function",  "colorMap",   "unused1",    "unused2",    "normalMap",  "unused3",    "unused4",    "specularMap", "unused5",
        "occlusionMap", "unused6",   "color0Map",  "color1Map",  "color2Map",  "color3Map",  "color4Map",  "color5Map",  "color6Map",   "color7Map",
        "color8Map",    "color9Map", "color10Map", "color11Map", "color12Map", "color13Map", "color14Map", "color15Map", "throwMap",
    };
    static_assert(std::extent_v<decltype(textureSemanticNames)> == TS_COUNT);

    inline const char* cameraRegionNames[]{
        "litOpaque",
        "litTrans",
        "litQuasiOpaque",
        "emissiveOpaque",
        "emissiveTrans",
        "emissiveFx",
        "lightMapOpaque",
        "depthHack",
        "unused",
        "sonar",
        "none",
    };
    static_assert(std::extent_v<decltype(cameraRegionNames)> == CAMERA_REGION_NONE + 1);

    static const char* textureFilterNames[]{
        "disabled",
        "nearest",
        "linear",
        "aniso2x",
        "aniso4x",
        "compare",
    };
    static_assert(std::extent_v<decltype(textureFilterNames)> == TEXTURE_FILTER_COUNT);

    static const char* samplerStateMipMapNames[]{
        "disabled",
        "nearest",
        "linear",
    };
    static_assert(std::extent_v<decltype(samplerStateMipMapNames)> == SAMPLER_MIPMAP_ENUM_COUNT);

    static const char* gameFlagNames[]{
        "1",
        "2",
        "NO_MARKS",
        "8",
        "10",
        "20",
        "CASTS_SHADOW",
        "80",
        "100",
        "200",
        "400",
        "800",
        "1000",
    };

    inline const char* blendNames[]{
        "disabled",
        "zero",
        "one",
        "srccolor",
        "invsrccolor",
        "srcalpha",
        "invsrcalpha",
        "destalpha",
        "invdestalpha",
        "destcolor",
        "invdestcolor",
    };
    static_assert(std::extent_v<decltype(blendNames)> == GFXS_BLEND_COUNT);

    inline const char* blendOpNames[]{
        "disabled",
        "add",
        "subtract",
        "revsubtract",
        "min",
        "max",
    };
    static_assert(std::extent_v<decltype(blendOpNames)> == GFXS_BLENDOP_COUNT);

    inline const char* polygonOffsetNames[]{
        "offset0",
        "offset1",
        "offset2",
        "offsetShadowmap",
    };
    static_assert(std::extent_v<decltype(polygonOffsetNames)> == GFXS_POLYGON_OFFSET_COUNT);

    inline const char* stencilOpNames[]{
        "keep",
        "zero",
        "replace",
        "incrsat",
        "decrsat",
        "invert",
        "incr",
        "decr",
    };
    static_assert(std::extent_v<decltype(stencilOpNames)> == GFXS_STENCILOP_COUNT);

    inline const char* stencilFuncNames[]{
        "never",
        "less",
        "equal",
        "lessequal",
        "greater",
        "notequal",
        "greaterequal",
        "always",
    };
    static_assert(std::extent_v<decltype(stencilFuncNames)> == GFXS_STENCILFUNC_COUNT);
} // namespace T6
