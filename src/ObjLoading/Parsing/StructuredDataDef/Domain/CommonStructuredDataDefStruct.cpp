#include "CommonStructuredDataDefStruct.h"

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry()
    : m_offset_in_bits(0u)
{
}

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry(std::string name)
    : m_name(std::move(name)),
      m_offset_in_bits(0u)
{
}

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry(std::string name, const CommonStructuredDataDefType type, const size_t offsetInBits)
    : m_name(std::move(name)),
      m_type(type),
      m_offset_in_bits(offsetInBits)
{
}

CommonStructuredDataDefStruct::CommonStructuredDataDefStruct()
    : m_bit_offset(0u),
      m_size_in_byte(0u)
{
}

CommonStructuredDataDefStruct::CommonStructuredDataDefStruct(std::string name)
    : m_name(std::move(name)),
      m_bit_offset(0u),
      m_size_in_byte(0u)
{
}
