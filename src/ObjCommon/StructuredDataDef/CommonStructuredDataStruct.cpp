#include "CommonStructuredDataStruct.h"

#include <algorithm>

CommonStructuredDataStructEntry::CommonStructuredDataStructEntry()
    : m_offset_in_bits(0u)
{
}

CommonStructuredDataStructEntry::CommonStructuredDataStructEntry(std::string name)
    : m_name(std::move(name)),
      m_offset_in_bits(0u)
{
}

CommonStructuredDataStructEntry::CommonStructuredDataStructEntry(std::string name, const CommonStructuredDataType type, const size_t offsetInBits)
    : m_name(std::move(name)),
      m_type(type),
      m_offset_in_bits(offsetInBits)
{
}

CommonStructuredDataStruct::CommonStructuredDataStruct()
    : m_bit_offset(0u),
      m_size_in_byte(0u)
{
}

CommonStructuredDataStruct::CommonStructuredDataStruct(std::string name)
    : m_name(std::move(name)),
      m_bit_offset(0u),
      m_size_in_byte(0u)
{
}

void CommonStructuredDataStruct::SortProperties()
{
    std::sort(m_properties.begin(), m_properties.end(), [](const CommonStructuredDataStructEntry& e1, const CommonStructuredDataStructEntry& e2)
    {
        return e1.m_offset_in_bits < e2.m_offset_in_bits;
    });
}
