#include "InvalidOffsetBlockException.h"

InvalidOffsetBlockException::InvalidOffsetBlockException(const block_t referencedBlock)
{
    m_referenced_block = referencedBlock;
}

std::string InvalidOffsetBlockException::DetailedMessage()
{
    return "Zone tried to reference invalid block " + std::to_string(m_referenced_block);
}

char const* InvalidOffsetBlockException::what() const
{
    return "Zone referenced invalid block";
}