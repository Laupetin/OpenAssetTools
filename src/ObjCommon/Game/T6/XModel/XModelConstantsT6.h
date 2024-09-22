#pragma once

#include "Game/T6/T6.h"

namespace T6
{
    inline const char* HITLOC_NAMES[]{
        // clang-format off
        "none",
        "helmet",
        "head",
        "neck",
        "torso_upper",
        "torso_middle",
        "torso_lower",
        "right_arm_upper",
        "left_arm_upper",
        "right_arm_lower",
        "left_arm_lower",
        "right_hand",
        "left_hand",
        "right_leg_upper",
        "left_leg_upper",
        "right_leg_lower",
        "left_leg_lower",
        "right_foot",
        "left_foot",
        "gun",
        "shield",
        // clang-format on
    };
    static_assert(std::extent_v<decltype(HITLOC_NAMES)> == HITLOC_COUNT);
} // namespace T6
