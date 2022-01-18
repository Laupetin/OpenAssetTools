#pragma once

#include <cstddef>

enum class CommonStructuredDataDefTypeCategory
{
    UNKNOWN,
    INT,
    BYTE,
    BOOL,
    STRING,
    FLOAT,
    SHORT,
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
};

struct CommonStructuredDataDefIndexedArray
{
    CommonStructuredDataDefType m_array_type;
    size_t m_array_size;

    CommonStructuredDataDefIndexedArray();
    CommonStructuredDataDefIndexedArray(CommonStructuredDataDefType type, size_t arraySize);
};

struct CommonStructuredDataDefEnumedArray
{
    CommonStructuredDataDefType m_array_type;
    size_t m_enum_index;

    CommonStructuredDataDefEnumedArray();
    CommonStructuredDataDefEnumedArray(CommonStructuredDataDefType type, size_t enumIndex);
};