#pragma once

#include <string>
#include <vector>

#include "Utils/ClassUtils.h"

struct CommonStructuredDataDefEnumEntry
{
    std::string m_name;
    size_t m_value;

    CommonStructuredDataDefEnumEntry();
    CommonStructuredDataDefEnumEntry(std::string name, size_t value);
};

struct CommonStructuredDataDefEnum
{
    std::string m_name;
    int m_reserved_entry_count;
    std::vector<CommonStructuredDataDefEnumEntry> m_entries;

    CommonStructuredDataDefEnum();
    explicit CommonStructuredDataDefEnum(std::string name);
    CommonStructuredDataDefEnum(std::string name, int reservedEntryCount);

    _NODISCARD size_t ElementCount() const;
};