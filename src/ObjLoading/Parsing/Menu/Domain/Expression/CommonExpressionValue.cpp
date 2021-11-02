#include "CommonExpressionValue.h"

#include <cmath>

using namespace menu;

CommonExpressionValue::CommonExpressionValue(std::string stringValue)
    : m_type(Type::STRING),
      m_string_value(std::make_shared<std::string>(std::move(stringValue)))
{
    m_double_value = 0;
    m_int_value = 0;
}

CommonExpressionValue::CommonExpressionValue(const double doubleValue)
    : m_type(Type::DOUBLE)
{
    m_int_value = 0;
    m_double_value = doubleValue;
}

CommonExpressionValue::CommonExpressionValue(const int intValue)
    : m_type(Type::INT)
{
    m_double_value = 0;
    m_int_value = intValue;
}

bool CommonExpressionValue::IsStatic()
{
    return true;
}

CommonExpressionValue CommonExpressionValue::Evaluate()
{
    return *this;
}

bool CommonExpressionValue::IsTruthy() const
{
    if (m_type == Type::DOUBLE)
        return std::fpclassify(m_double_value) != FP_ZERO;
    if (m_type == Type::INT)
        return m_int_value != 0;
    if (m_type == Type::STRING)
        return m_string_value && !m_string_value->empty();
    return false;
}
