#include "CommonStructuredDataDefTypes.h"

CommonStructuredDataDefType::CommonStructuredDataDefType()
    : m_category(CommonStructuredDataDefTypeCategory::UNKNOWN),
      m_info({0})
{
}

CommonStructuredDataDefType::CommonStructuredDataDefType(const CommonStructuredDataDefTypeCategory category)
    : m_category(category),
      m_info({0})
{
}

CommonStructuredDataDefType::CommonStructuredDataDefType(const CommonStructuredDataDefTypeCategory category, const size_t extraInfo)
    : m_category(category),
      m_info({extraInfo})
{
}

bool operator<(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs)
{
    if (lhs.m_category < rhs.m_category)
        return true;
    if (rhs.m_category < lhs.m_category)
        return false;
    return lhs.m_info.type_index < rhs.m_info.type_index;
}

bool operator<=(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataDefType& lhs, const CommonStructuredDataDefType& rhs)
{
    return !(lhs < rhs);
}

CommonStructuredDataDefIndexedArray::CommonStructuredDataDefIndexedArray()
    : m_array_size(0u)
{
}

CommonStructuredDataDefIndexedArray::CommonStructuredDataDefIndexedArray(const CommonStructuredDataDefType type, const size_t arraySize)
    : m_array_type(type),
      m_array_size(arraySize)
{
}

bool operator<(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs)
{
    if (lhs.m_array_type < rhs.m_array_type)
        return true;
    if (rhs.m_array_type < lhs.m_array_type)
        return false;
    return lhs.m_array_size < rhs.m_array_size;
}

bool operator<=(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataDefIndexedArray& lhs, const CommonStructuredDataDefIndexedArray& rhs)
{
    return !(lhs < rhs);
}

CommonStructuredDataDefEnumedArray::CommonStructuredDataDefEnumedArray()
    : m_enum_index(0u)
{
}

CommonStructuredDataDefEnumedArray::CommonStructuredDataDefEnumedArray(const CommonStructuredDataDefType type, const size_t enumIndex)
    : m_array_type(type),
      m_enum_index(enumIndex)
{
}

bool operator<(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs)
{
    if (lhs.m_array_type < rhs.m_array_type)
        return true;
    if (rhs.m_array_type < lhs.m_array_type)
        return false;
    return lhs.m_enum_index < rhs.m_enum_index;
}

bool operator<=(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs)
{
    return rhs < lhs;
}

bool operator>=(const CommonStructuredDataDefEnumedArray& lhs, const CommonStructuredDataDefEnumedArray& rhs)
{
    return !(lhs < rhs);
}
