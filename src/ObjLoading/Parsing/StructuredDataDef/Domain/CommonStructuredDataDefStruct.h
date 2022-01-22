#pragma once

#include <string>
#include <vector>

#include "CommonStructuredDataDefTypes.h"

struct CommonStructuredDataDefStructEntry
{
    std::string m_name;
    CommonStructuredDataDefType m_type;
    size_t m_offset_in_bits;

    CommonStructuredDataDefStructEntry();
    explicit CommonStructuredDataDefStructEntry(std::string name);
    CommonStructuredDataDefStructEntry(std::string name, CommonStructuredDataDefType type, size_t offsetInBits);
};

struct CommonStructuredDataDefStruct
{
    std::string m_name;
    std::vector<CommonStructuredDataDefStructEntry> m_properties;
    size_t m_bit_offset;
    size_t m_size_in_byte;

    CommonStructuredDataDefStruct();
    explicit CommonStructuredDataDefStruct(std::string name);
};