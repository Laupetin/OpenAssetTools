#pragma once

#include "WordSize.h"

#include <bit>
#include <string>

class GameVariant
{
public:
    GameVariant(std::string name, WordSize wordSize, std::endian endianness);

    std::string m_name;
    WordSize m_word_size;
    std::endian m_endianness;
};
