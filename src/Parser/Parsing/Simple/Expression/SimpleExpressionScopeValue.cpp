#include "SimpleExpressionScopeValue.h"

SimpleExpressionScopeValue::SimpleExpressionScopeValue(std::string valueName)
    : m_value_name(std::move(valueName))
{
}

bool SimpleExpressionScopeValue::Equals(const ISimpleExpression* other) const
{
    const auto* otherScopeValue = dynamic_cast<const SimpleExpressionScopeValue*>(other);

    return otherScopeValue && otherScopeValue->m_value_name == m_value_name;
}

bool SimpleExpressionScopeValue::IsStatic() const
{
    return false;
}

SimpleExpressionValue SimpleExpressionScopeValue::EvaluateStatic() const
{
    return SimpleExpressionValue(0);
}

SimpleExpressionValue SimpleExpressionScopeValue::EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const
{
    return scopeValues->ValueByName(m_value_name);
}
