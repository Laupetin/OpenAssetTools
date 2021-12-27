#pragma once
#include "ISimpleExpression.h"

class SimpleExpressionConditionalOperator final : public ISimpleExpression
{
public:
    std::unique_ptr<ISimpleExpression> m_condition;
    std::unique_ptr<ISimpleExpression> m_true_value;
    std::unique_ptr<ISimpleExpression> m_false_value;

    _NODISCARD bool Equals(const ISimpleExpression* other) const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD SimpleExpressionValue Evaluate() const override;

    SimpleExpressionConditionalOperator();
    SimpleExpressionConditionalOperator(std::unique_ptr<ISimpleExpression> condition, std::unique_ptr<ISimpleExpression> trueExpression, std::unique_ptr<ISimpleExpression> falseExpression);
};
