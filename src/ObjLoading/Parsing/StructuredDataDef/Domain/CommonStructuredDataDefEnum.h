#pragma once
#include <string>
#include <vector>

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
    std::vector<CommonStructuredDataDefEnumEntry> m_entries;

    CommonStructuredDataDefEnum();
    explicit CommonStructuredDataDefEnum(std::string name);
};