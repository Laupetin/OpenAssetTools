#pragma once

#include "Game/IW5/IW5.h"

namespace IW5
{
    inline const char* s_vehicleClassNames[]{
        "4 wheel",
        "tank",
        "plane",
        "boat",
        "artillery",
        "helicopter",
        "snowmobile",
        "submarine",
        "ugv",
    };
    static_assert(std::extent_v<decltype(s_vehicleClassNames)> == VEH_TYPE_COUNT);

    inline const char* s_vehicleAxleTypeNames[]{
        "front",
        "rear",
        "all",
    };
    static_assert(std::extent_v<decltype(s_vehicleAxleTypeNames)> == VEH_AXLE_COUNT);

    inline const char* s_vehicleCameraZOffsetModeNames[]{
        "world",
        "vehicle",
        "view",
    };
    static_assert(std::extent_v<decltype(s_vehicleCameraZOffsetModeNames)> == VEHCAM_ZMODE_COUNT);
} // namespace IW5
