#include "GameVariant.h"

GameVariant::GameVariant(std::string name, const WordSize wordSize, const std::endian endianness)
    : m_name(std::move(name)),
      m_word_size(wordSize),
      m_endianness(endianness)
{
}
