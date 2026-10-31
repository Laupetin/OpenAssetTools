#pragma once

namespace fx
{
    enum class FxFlagType
    {
        EDITOR,
        NATIVE,
        ATLAS,
        EFFECT,
    };

    struct FxFlagDef
    {
        const char* name;
        FxFlagType type;
        unsigned int mask;
        unsigned int value;
    };
} // namespace fx
