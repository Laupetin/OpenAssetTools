#pragma once

#include <cstdint>
#include <utility>

enum class WordSize : std::uint8_t
{
    BITS_32,
    BITS_64,

    COUNT
};
static constexpr auto WORD_SIZE_COUNT = std::to_underlying(WordSize::COUNT);

static constexpr WordSize OWN_WORD_SIZE =
#if defined(ARCH_x86)
    WordSize::BITS_32
#elif defined(ARCH_x64)
    WordSize::BITS_64
#endif
    ;

extern unsigned GetPointerSizeForWordSize(WordSize wordSize);
