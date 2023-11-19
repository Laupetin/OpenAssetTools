#include "InvalidOffsetBlockOffsetException.h"

InvalidOffsetBlockOffsetException::InvalidOffsetBlockOffsetException(XBlock* block, const size_t referencedOffset)
{
    m_referenced_block = block;
    m_referenced_offset = referencedOffset;
}

std::string InvalidOffsetBlockOffsetException::DetailedMessage()
{
    return "Zone referenced offset " + std::to_string(m_referenced_offset) + " of block " + m_referenced_block->m_name + " which is larger than its size "
           + std::to_string(m_referenced_block->m_buffer_size);
}

char const* InvalidOffsetBlockOffsetException::what() const noexcept
{
    return "Zone referenced offset of block that is out of bounds";
}
