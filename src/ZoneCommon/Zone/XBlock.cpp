#include "XBlock.h"

XBlock::XBlock(std::string name, const unsigned index, const XBlockType type)
    : m_name(std::move(name)),
      m_index(index),
      m_type(type),
      m_buffer_size(0u)
{
}

void XBlock::Alloc(const size_t blockSize)
{
    if (blockSize > 0)
    {
        m_buffer = std::make_unique<uint8_t[]>(blockSize);
        m_buffer_size = blockSize;
    }
    else
    {
        m_buffer.reset();
        m_buffer_size = 0;
    }
}
