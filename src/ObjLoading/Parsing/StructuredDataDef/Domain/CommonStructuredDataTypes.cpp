#include "CommonStructuredDataTypes.h"

CommonStructuredDataType::CommonStructuredDataType()
    : m_category(CommonStructuredDataTypeCategory::UNKNOWN),
      m_info({0})
{
}

CommonStructuredDataType::CommonStructuredDataType(const CommonStructuredDataTypeCategory category)
    : m_category(category),
      m_info({0})
{
}

CommonStructuredDataType::CommonStructuredDataType(const CommonStructuredDataTypeCategory category, const size_t extraInfo)
    : m_category(category),
      m_info({extraInfo})
{
}

bool operator<(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs)
{
    if (lhs.m_category < rhs.m_category)
        return true;
    if (rhs.m_category < lhs.m_category)
        return false;
    return lhs.m_info.type_index < rhs.m_info.type_index;
}

bool operator<=(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataType& lhs, const CommonStructuredDataType& rhs)
{
    return !(lhs < rhs);
}

CommonStructuredDataIndexedArray::CommonStructuredDataIndexedArray()
    : CommonStructuredDataIndexedArray({}, 0u)
{
}

CommonStructuredDataIndexedArray::CommonStructuredDataIndexedArray(const CommonStructuredDataType type, const size_t elementCount)
    : CommonStructuredDataIndexedArray(type, elementCount, 0u)
{
}

CommonStructuredDataIndexedArray::CommonStructuredDataIndexedArray(const CommonStructuredDataType type, const size_t elementCount, const size_t elementSize)
    : m_array_type(type),
      m_element_count(elementCount),
      m_element_size_in_bits(elementSize)
{
}

bool operator<(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs)
{
    if (lhs.m_array_type < rhs.m_array_type)
        return true;
    if (rhs.m_array_type < lhs.m_array_type)
        return false;
    return lhs.m_element_count < rhs.m_element_count;
}

bool operator<=(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataIndexedArray& lhs, const CommonStructuredDataIndexedArray& rhs)
{
    return !(lhs < rhs);
}

CommonStructuredDataEnumedArray::CommonStructuredDataEnumedArray()
    : CommonStructuredDataEnumedArray({}, 0u)
{
}

CommonStructuredDataEnumedArray::CommonStructuredDataEnumedArray(const CommonStructuredDataType type, const size_t enumIndex)
    : CommonStructuredDataEnumedArray(type, enumIndex, 0u, 0u)
{
}

CommonStructuredDataEnumedArray::CommonStructuredDataEnumedArray(const CommonStructuredDataType type, const size_t enumIndex, const size_t elementCount, const size_t elementSizeInBits)
    : m_array_type(type),
      m_enum_index(enumIndex),
      m_element_count(elementCount),
      m_element_size_in_bits(elementSizeInBits)
{
}

bool operator<(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs)
{
    if (lhs.m_array_type < rhs.m_array_type)
        return true;
    if (rhs.m_array_type < lhs.m_array_type)
        return false;
    return lhs.m_enum_index < rhs.m_enum_index;
}

bool operator<=(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataEnumedArray& lhs, const CommonStructuredDataEnumedArray& rhs)
{
    return !(lhs < rhs);
}
