#include "CommonStructuredDataEnum.h"

CommonStructuredDataEnumEntry::CommonStructuredDataEnumEntry()
    : m_value(0u)
{
}

CommonStructuredDataEnumEntry::CommonStructuredDataEnumEntry(std::string name, const size_t value)
    : m_name(std::move(name)),
      m_value(value)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum()
    : m_reserved_entry_count(-1)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum(std::string name)
    : m_name(std::move(name)),
      m_reserved_entry_count(-1)
{
}

CommonStructuredDataEnum::CommonStructuredDataEnum(std::string name, const int reservedEntryCount)
    : m_name(std::move(name)),
      m_reserved_entry_count(reservedEntryCount)
{
}

size_t CommonStructuredDataEnum::ElementCount() const
{
    return m_reserved_entry_count > 0 ? static_cast<size_t>(m_reserved_entry_count) : m_entries.size();
}
