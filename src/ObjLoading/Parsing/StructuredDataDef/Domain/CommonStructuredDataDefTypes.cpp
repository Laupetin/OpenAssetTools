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

CommonStructuredDataDefIndexedArray::CommonStructuredDataDefIndexedArray()
    : m_array_size(0u)
{
}

CommonStructuredDataDefIndexedArray::CommonStructuredDataDefIndexedArray(const CommonStructuredDataDefType type, const size_t arraySize)
    : m_array_type(type),
      m_array_size(arraySize)
{
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
