#include "CommonStructuredDataStruct.h"

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
