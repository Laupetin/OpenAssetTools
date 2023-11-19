#pragma once

#include "Game/IW3/IW3.h"

#include <type_traits>
#include <unordered_map>

namespace IW3
{
    inline const char* surfaceTypeNames[]{
        "default", "bark",   "brick",   "carpet",  "cloth",   "concrete", "dirt",    "flesh", "foliage",      "glass",
        "grass",   "gravel", "ice",     "metal",   "mud",     "paper",    "plaster", "rock",  "sand",         "snow",
        "water",   "wood",   "asphalt", "ceramic", "plastic", "rubber",   "cushion", "fruit", "paintedmetal",
    };
    static_assert(std::extent_v<decltype(surfaceTypeNames)> == SURF_TYPE_NUM);
} // namespace IW3
