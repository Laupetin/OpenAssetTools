#pragma once
#include "Utils/ClassUtils.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct CommonStructuredDataEnumEntry
{
    std::string m_name;
    size_t m_value;

    CommonStructuredDataEnumEntry();
    CommonStructuredDataEnumEntry(std::string name, size_t value);
};

struct CommonStructuredDataEnum
{
    std::string m_name;
    int m_reserved_entry_count;
    std::vector<CommonStructuredDataEnumEntry> m_entries;

    CommonStructuredDataEnum();
    explicit CommonStructuredDataEnum(std::string name);
    CommonStructuredDataEnum(std::string name, int reservedEntryCount);

    _NODISCARD size_t ElementCount() const;
    _NODISCARD uint32_t CalculateChecksum(uint32_t initialValue) const;

    void SortEntriesByOffset();
    void SortEntriesByName();
};
