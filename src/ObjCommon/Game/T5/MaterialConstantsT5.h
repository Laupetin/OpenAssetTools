#pragma once

#include "Game/T5/T5.h"

#include <type_traits>

namespace T5
{
    inline const char* surfaceTypeNames[]{
        "default", "bark",    "brick",   "carpet", "cloth",   "concrete", "dirt",         "flesh",  "foliage",   "glass", "grass",
        "gravel",  "ice",     "metal",   "mud",    "paper",   "plaster",  "rock",         "sand",   "snow",      "water", "wood",
        "asphalt", "ceramic", "plastic", "rubber", "cushion", "fruit",    "paintedmetal", "player", "tallgrass",
    };
    static_assert(std::extent_v<decltype(surfaceTypeNames)> == SURF_TYPE_NUM);
} // namespace T5
