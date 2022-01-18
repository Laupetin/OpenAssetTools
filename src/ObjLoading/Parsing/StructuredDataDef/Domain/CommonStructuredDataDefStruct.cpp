#include "CommonStructuredDataDefStruct.h"

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry()
    : m_offset(0u)
{
}

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry(std::string name)
    : m_name(std::move(name)),
      m_offset(0u)
{
}

CommonStructuredDataDefStructEntry::CommonStructuredDataDefStructEntry(std::string name, const CommonStructuredDataDefType type, const size_t offset)
    : m_name(std::move(name)),
      m_type(type),
      m_offset(offset)
{
}

CommonStructuredDataDefStruct::CommonStructuredDataDefStruct()
    : m_size(0u)
{
}

CommonStructuredDataDefStruct::CommonStructuredDataDefStruct(std::string name)
    : m_name(std::move(name)),
      m_size(0u)
{
}
