#include "CommonStructuredDataDefEnum.h"

CommonStructuredDataDefEnumEntry::CommonStructuredDataDefEnumEntry()
    : m_value(0u)
{
}

CommonStructuredDataDefEnumEntry::CommonStructuredDataDefEnumEntry(std::string name, const size_t value)
    : m_name(std::move(name)),
      m_value(value)
{
}

CommonStructuredDataDefEnum::CommonStructuredDataDefEnum()
= default;

CommonStructuredDataDefEnum::CommonStructuredDataDefEnum(std::string name)
    : m_name(std::move(name))
{
}
