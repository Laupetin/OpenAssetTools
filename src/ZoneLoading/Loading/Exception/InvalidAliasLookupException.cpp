#include "InvalidAliasLookupException.h"

#include <format>

InvalidAliasLookupException::InvalidAliasLookupException(const size_t lookupIndex, const size_t lookupCount)
    : m_lookup_index(lookupIndex),
      m_lookup_count(lookupCount)
{
}

std::string InvalidAliasLookupException::DetailedMessage()
{
    return std::format("Tried to resolve zone alias lookup {} when there are only {} entries in the lookup", m_lookup_index, m_lookup_count);
}

char const* InvalidAliasLookupException::what() const noexcept
{
    return "Tried to resolve invalid zone alias lookup";
}
