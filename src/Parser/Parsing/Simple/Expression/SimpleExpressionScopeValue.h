#pragma once

#include <string>

#include "ISimpleExpression.h"
#include "SimpleExpressionValue.h"

class SimpleExpressionScopeValue final : public ISimpleExpression
{
public:
    std::string m_value_name;

    explicit SimpleExpressionScopeValue(std::string valueName);

    _NODISCARD bool Equals(const ISimpleExpression* other) const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
};
