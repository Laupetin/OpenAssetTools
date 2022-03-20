#pragma once

#include <cstddef>

#include "Utils/ClassUtils.h"

enum class CommonStructuredDataTypeCategory
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

union CommonStructuredDataTypeExtraInfo
{
    size_t string_length;
    size_t type_index;
};

class CommonStructuredDataDef;
struct CommonStructuredDataType
{
    CommonStructuredDataTypeCategory m_category;
    CommonStructuredDataTypeExtraInfo m_info;

    CommonStructuredDataType();
    explicit CommonStructuredDataType(CommonStructuredDataTypeCategory category);
    CommonStructuredDataType(CommonStructuredDataTypeCategory category, size_t extraInfo);

    _NODISCARD size_t GetAlignmentInBits() const;
    _NODISCARD size_t GetSizeInBits(const CommonStructuredDataDef& def) const;

    friend bool operator<(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs);
    friend bool operator<=(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs);
    friend bool operator>(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs);
    friend bool operator>=(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs);
};

struct CommonStructuredDataIndexedArray
{
    CommonStructuredDataType m_array_type;
    size_t m_element_count;
    size_t m_element_size_in_bits;

    CommonStructuredDataIndexedArray();
    CommonStructuredDataIndexedArray(CommonStructuredDataType type, size_t elementCount);
    CommonStructuredDataIndexedArray(CommonStructuredDataType type, size_t elementCount, size_t elementSize);

    friend bool operator<(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs);
    friend bool operator<=(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs);
    friend bool operator>(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs);
    friend bool operator>=(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs);
};

struct CommonStructuredDataEnumedArray
{
    CommonStructuredDataType m_array_type;
    size_t m_enum_index;
    size_t m_element_count;
    size_t m_element_size_in_bits;

    CommonStructuredDataEnumedArray();
    CommonStructuredDataEnumedArray(CommonStructuredDataType type, size_t enumIndex);
    CommonStructuredDataEnumedArray(CommonStructuredDataType type, size_t enumIndex, size_t elementCount, size_t elementSizeInBits);

    friend bool operator<(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs);
    friend bool operator<=(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs);
    friend bool operator>(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs);
    friend bool operator>=(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs);
};
