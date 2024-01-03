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
} // namespace T6
