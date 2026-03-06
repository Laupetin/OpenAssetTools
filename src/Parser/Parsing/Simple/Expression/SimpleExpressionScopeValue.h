#pragma once

#include "ISimpleExpression.h"
#include "SimpleExpressionValue.h"

#include <string>

class SimpleExpressionScopeValue final : public ISimpleExpression
{
public:
    std::string m_value_name;

    explicit SimpleExpressionScopeValue(std::string valueName);

    [[nodiscard]] bool Equals(const ISimpleExpression* other) const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
};
