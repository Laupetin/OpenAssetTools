#pragma once

#include <unordered_map>
#include <type_traits>

#include "Game/IW3/IW3.h"

namespace IW3
{
    inline const char* surfaceTypeNames[]
    {
        "default",
        "bark",
        "brick",
        "carpet",
        "cloth",
        "concrete",
        "dirt",
        "flesh",
        "foliage",
        "glass",
        "grass",
        "gravel",
        "ice",
        "metal",
        "mud",
        "paper",
        "plaster",
        "rock",
        "sand",
        "snow",
        "water",
        "wood",
        "asphalt",
        "ceramic",
        "plastic",
        "rubber",
        "cushion",
        "fruit",
        "paintedmetal"
    };
    static_assert(std::extent_v<decltype(surfaceTypeNames)> == SURF_TYPE_NUM);
}
