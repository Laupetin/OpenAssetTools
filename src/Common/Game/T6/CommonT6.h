#pragma once

namespace T6
{
    inline const char* szAttachmentTypeNames[]
    {
        "none",
        "acog",
        "dualclip",
        "dualoptic",
        "dw",
        "extbarrel",
        "extclip",
        "extramags",
        "fastads",
        "fastreload",
        "fmj",
        "gl",
        "grip",
        "holo",
        "ir",
        "is",
        "longbreath",
        "mk",
        "mms",
        "rangefinder",
        "reflex",
        "rf",
        "sf",
        "silencer",
        "stackfire",
        "stalker",
        "steadyaim",
        "swayreduc",
        "tacknife",
        "vzoom"
    };

    inline const char* szWeapFireTypeNames[]
    {
        "Full Auto",
        "Single Shot",
        "2-Round Burst",
        "3-Round Burst",
        "4-Round Burst",
        "5-Round Burst",
        "Stacked Fire",
        "Minigun",
        "Charge Shot",
        "Jetgun"
    };

    inline const char* penetrateTypeNames[]
    {
        "none",
        "small",
        "medium",
        "large"
    };

    inline const char* s_constraintTypeNames[]
    {
        "none",
        "point",
        "distance",
        "hinge",
        "joint",
        "actuator",
        "fake_shake",
        "launch",
        "rope",
        "light",
    };
}

class CommonT6
{
public:
    static int Com_HashKey(const char* str, int maxLen);
    static int Com_HashString(const char* str, int len);
};