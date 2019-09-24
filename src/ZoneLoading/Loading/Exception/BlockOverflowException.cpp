#include "BlockOverflowException.h"

BlockOverflowException::BlockOverflowException(XBlock* block)
{
    m_block = block;
}

std::string BlockOverflowException::DetailedMessage()
{
    return "XBlock " + m_block->m_name + " overflowed while trying to load zone.";
}

char const* BlockOverflowException::what() const
{
    return "Invalid Zone. XBlock overflowed.";
}