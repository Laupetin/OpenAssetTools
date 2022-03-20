#pragma once

#include <string>
#include <vector>

#include "Utils/ClassUtils.h"
#include "CommonStructuredDataTypes.h"

struct CommonStructuredDataStructEntry
{
    std::string m_name;
    CommonStructuredDataType m_type;
    size_t m_offset_in_bits;

    CommonStructuredDataStructEntry();
    explicit CommonStructuredDataStructEntry(std::string name);
    CommonStructuredDataStructEntry(std::string name, CommonStructuredDataType type, size_t offsetInBits);
};

class CommonStructuredDataDef;
struct CommonStructuredDataStruct
{
    std::string m_name;
    std::vector<CommonStructuredDataStructEntry> m_properties;
    size_t m_bit_offset;
    size_t m_size_in_byte;

    CommonStructuredDataStruct();
    explicit CommonStructuredDataStruct(std::string name);

    _NODISCARD uint32_t CalculateChecksum(const CommonStructuredDataDef& def, uint32_t initialValue) const;

    void SortProperties();
};
