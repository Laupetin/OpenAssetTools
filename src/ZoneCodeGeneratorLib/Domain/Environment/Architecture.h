#pragma once

enum class Architecture
{
    UNKNOWN,
    X86,
    X64
};

static constexpr Architecture OWN_ARCHITECTURE =
#if defined(ARCH_x86)
    Architecture::X86
#elif defined(ARCH_x64)
    Architecture::X64
#endif
    ;
