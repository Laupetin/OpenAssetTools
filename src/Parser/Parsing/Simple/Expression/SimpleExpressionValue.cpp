#include "SimpleExpressionValue.h"

#include <cmath>

SimpleExpressionValue::SimpleExpressionValue(std::string stringValue)
    : m_type(Type::STRING),
      m_string_value(std::make_shared<std::string>(std::move(stringValue)))
{
    m_double_value = 0;
    m_int_value = 0;
}

SimpleExpressionValue::SimpleExpressionValue(const double doubleValue)
    : m_type(Type::DOUBLE)
{
    m_int_value = 0;
    m_double_value = doubleValue;
}

SimpleExpressionValue::SimpleExpressionValue(const int intValue)
    : m_type(Type::INT)
{
    m_double_value = 0;
    m_int_value = intValue;
}

bool SimpleExpressionValue::Equals(const ISimpleExpression* other) const
{
    const auto* otherExpressionValue = dynamic_cast<const SimpleExpressionValue*>(other);

    if (!otherExpressionValue || m_type != otherExpressionValue->m_type)
        return false;

    if (m_type == Type::INT)
        return m_int_value == otherExpressionValue->m_int_value;
    if (m_type == Type::DOUBLE)
        return std::fabs(m_double_value - otherExpressionValue->m_double_value) < std::numeric_limits<double>::epsilon();
    if (m_type == Type::STRING)
        return *m_string_value == *otherExpressionValue->m_string_value;

    return true;
}

bool SimpleExpressionValue::IsStatic() const
{
    return true;
}

SimpleExpressionValue SimpleExpressionValue::EvaluateStatic() const
{
    return *this;
}

SimpleExpressionValue SimpleExpressionValue::EvaluateNonStatic(ISimpleExpressionScopeValues* scopeValues) const
{
    return *this;
}

bool SimpleExpressionValue::IsTruthy() const
{
    if (m_type == Type::DOUBLE)
        return std::fpclassify(m_double_value) != FP_ZERO;
    if (m_type == Type::INT)
        return m_int_value != 0;
    if (m_type == Type::STRING)
        return m_string_value && !m_string_value->empty();
    return false;
}
