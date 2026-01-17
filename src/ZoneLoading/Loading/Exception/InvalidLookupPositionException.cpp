#include "InvalidLookupPositionException.h"

#include <format>

InvalidLookupPositionException::InvalidLookupPositionException(const block_t block, const size_t offset)
    : m_block(block),
      m_offset(offset)
{
}

std::string InvalidLookupPositionException::DetailedMessage()
{
    return std::format("Zone tried to lookup at block {}, offset {} that was not recorded", m_block, m_offset);
}

char const* InvalidLookupPositionException::what() const noexcept
{
    return "Zone tried to lookup at zone offset that is not recorded";
}
