#include "XBlock.h"

#include <new>

namespace
{
    constexpr std::align_val_t XBLOCK_BUFFER_ALIGNMENT{16u};
}

void XBlockBufferDeleter::operator()(std::uint8_t* buffer) const noexcept
{
    ::operator delete[](buffer, XBLOCK_BUFFER_ALIGNMENT);
}

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
        m_buffer.reset(static_cast<std::uint8_t*>(::operator new[](blockSize, XBLOCK_BUFFER_ALIGNMENT)));
        m_buffer_size = blockSize;
    }
    else
    {
        m_buffer.reset();
        m_buffer_size = 0;
    }
}
