#pragma once

#include <string>
#include <vector>

#include "CommonStructuredDataDefTypes.h"

struct CommonStructuredDataDefStructEntry
{
    std::string m_name;
    CommonStructuredDataDefType m_type;
    size_t m_offset;

    CommonStructuredDataDefStructEntry();
    explicit CommonStructuredDataDefStructEntry(std::string name);
    CommonStructuredDataDefStructEntry(std::string name, CommonStructuredDataDefType type, size_t offset);
};

struct CommonStructuredDataDefStruct
{
    std::string m_name;
    std::vector<CommonStructuredDataDefStructEntry> m_entries;
    size_t m_size;

    CommonStructuredDataDefStruct();
    explicit CommonStructuredDataDefStruct(std::string name);
};