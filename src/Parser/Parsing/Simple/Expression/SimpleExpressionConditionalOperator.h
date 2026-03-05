#pragma once

#include "ISimpleExpression.h"

class SimpleExpressionConditionalOperator final : public ISimpleExpression
{
public:
    std::unique_ptr<ISimpleExpression> m_condition;
    std::unique_ptr<ISimpleExpression> m_true_value;
    std::unique_ptr<ISimpleExpression> m_false_value;

    [[nodiscard]] bool Equals(const ISimpleExpression* other) const override;
    [[nodiscard]] bool IsStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateStatic() const override;
    [[nodiscard]] SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;

    SimpleExpressionConditionalOperator();
    SimpleExpressionConditionalOperator(std::unique_ptr<ISimpleExpression> condition,
                                        std::unique_ptr<ISimpleExpression> trueExpression,
                                        std::unique_ptr<ISimpleExpression> falseExpression);
};
