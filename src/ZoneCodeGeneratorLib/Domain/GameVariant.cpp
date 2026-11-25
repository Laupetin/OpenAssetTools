#include "GameVariant.h"

GameVariant::GameVariant(const unsigned index, std::string name, const WordSize wordSize, const std::endian endianness)
    : m_index(index),
      m_name(std::move(name)),
      m_word_size(wordSize),
      m_endianness(endianness)
{
}
