#include "FastFileBlock.h"

FastFileBlock::FastFileBlock(std::string name, const unsigned index, const FastFileBlockType type, const bool isDefault)
    : m_name(std::move(name)),
      m_index(index),
      m_type(type),
      m_is_default(isDefault)
{
}
