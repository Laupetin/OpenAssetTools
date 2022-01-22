#pragma once

#include <cstddef>

enum class CommonStructuredDataDefTypeCategory
{
    UNKNOWN,
    INT,
    BYTE,
    BOOL,
    FLOAT,
    SHORT,
    STRING,
    ENUM,
    STRUCT,
    INDEXED_ARRAY,
    ENUM_ARRAY
};

union CommonStructuredDataDefTypeExtraInfo
{
    size_t string_length;
    size_t type_index;
};

struct CommonStructuredDataDefType
{
    CommonStructuredDataDefTypeCategory m_category;
    CommonStructuredDataDefTypeExtraInfo m_info;

    CommonStructuredDataDefType();
    explicit CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory category);
    CommonStructuredDataDefType(CommonStructuredDataDefTypeCategory category, size_t extraInfo);

    friend bool operator<(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs);
    friend bool operator<=(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs);
    friend bool operator>(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs);
    friend bool operator>=(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs);
};

struct CommonStructuredDataDefIndexedArray
{
    CommonStructuredDataDefType m_array_type;
    size_t m_element_count;
    size_t m_element_size_in_bits;

    CommonStructuredDataDefIndexedArray();
    CommonStructuredDataDefIndexedArray(CommonStructuredDataDefType type, size_t elementCount);
    CommonStructuredDataDefIndexedArray(CommonStructuredDataDefType type, size_t elementCount, size_t elementSize);

    friend bool operator<(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs);
    friend bool operator<=(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs);
    friend bool operator>(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs);
    friend bool operator>=(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs);
};

struct CommonStructuredDataDefEnumedArray
{
    CommonStructuredDataDefType m_array_type;
    size_t m_enum_index;
    size_t m_element_count;
    size_t m_element_size_in_bits;

    CommonStructuredDataDefEnumedArray();
    CommonStructuredDataDefEnumedArray(CommonStructuredDataDefType type, size_t enumIndex);
    CommonStructuredDataDefEnumedArray(CommonStructuredDataDefType type, size_t enumIndex, size_t elementCount, size_t elementSizeInBits);

    friend bool operator<(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs);
    friend bool operator<=(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs);
    friend bool operator>(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs);
    friend bool operator>=(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs);
};