#pragma once

#include "Game/IW4/IW4.h"

namespace IW4
{
    inline const char* s_vehicleClassNames[]{
        "4 wheel",
        "tank",
        "plane",
        "boat",
        "artillery",
        "helicopter",
        "snowmobile",
    };
    static_assert(std::extent_v<decltype(s_vehicleClassNames)> == VEH_TYPE_COUNT);

    inline const char* s_vehicleAxleTypeNames[]{
        "front",
        "rear",
        "all",
    };
    static_assert(std::extent_v<decltype(s_vehicleAxleTypeNames)> == VEH_AXLE_COUNT);
} // namespace IW4
