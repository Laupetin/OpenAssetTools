#pragma once

#include <cstdint>

enum class WordSize : std::uint8_t
{
    UNKNOWN,
    BITS_32,
    BITS_64
};

static constexpr WordSize OWN_WORD_SIZE =
#if defined(ARCH_x86)
    WordSize::BITS_32
#elif defined(ARCH_x64)
    WordSize::BITS_64
#endif
    ;

extern unsigned GetPointerSizeForWordSize(WordSize wordSize);
