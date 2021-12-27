#pragma once

#include <string>
#include <memory>
#include <functional>

#include "ISimpleExpression.h"
#include "SimpleExpressionValue.h"

enum class SimpleUnaryOperationId
{
    NOT,
    BITWISE_NOT,
    NEGATIVE,

    COUNT
};

class SimpleExpressionUnaryOperationType
{
public:
    using evaluation_function_t = std::function<SimpleExpressionValue(const SimpleExpressionValue& operand)>;

    SimpleUnaryOperationId m_id;
    std::string m_syntax;
    evaluation_function_t m_evaluation_function;

private:
    SimpleExpressionUnaryOperationType(SimpleUnaryOperationId id, std::string syntax, evaluation_function_t evaluationFunction);

public:
    static const SimpleExpressionUnaryOperationType OPERATION_NOT;
    static const SimpleExpressionUnaryOperationType OPERATION_BITWISE_NOT;
    static const SimpleExpressionUnaryOperationType OPERATION_NEGATIVE;

    static const SimpleExpressionUnaryOperationType* const ALL_OPERATION_TYPES[static_cast<int>(SimpleUnaryOperationId::COUNT)];
};

class SimpleExpressionUnaryOperation final : public ISimpleExpression
{
public:
    const SimpleExpressionUnaryOperationType* m_operation_type;
    std::unique_ptr<ISimpleExpression> m_operand;

    SimpleExpressionUnaryOperation(const SimpleExpressionUnaryOperationType* operationType, std::unique_ptr<ISimpleExpression> operand);

    _NODISCARD bool OperandNeedsParenthesis() const;

    _NODISCARD bool Equals(const ISimpleExpression* other) const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD SimpleExpressionValue Evaluate() const override;
};
