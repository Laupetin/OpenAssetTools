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
    : m_reserved_entry_count(-1)
{
}

CommonStructuredDataDefEnum::CommonStructuredDataDefEnum(std::string name)
    : m_name(std::move(name)),
      m_reserved_entry_count(-1)
{
}

CommonStructuredDataDefEnum::CommonStructuredDataDefEnum(std::string name, const int reservedEntryCount)
    : m_name(std::move(name)),
      m_reserved_entry_count(reservedEntryCount)
{
}
