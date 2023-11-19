#include "OutOfBlockBoundsException.h"

OutOfBlockBoundsException::OutOfBlockBoundsException(XBlock* block)
{
    m_block = block;
}

std::string OutOfBlockBoundsException::DetailedMessage()
{
    return "Tried to load to location out of bounds from current XBlock " + m_block->m_name + ".";
}

char const* OutOfBlockBoundsException::what() const noexcept
{
    return "Invalid Zone. Out of XBlock bounds.";
}
