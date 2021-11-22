#include "SimpleExpressionUnaryOperation.h"

#include <cassert>

#include "SimpleExpressionBinaryOperation.h"

SimpleExpressionUnaryOperationType::SimpleExpressionUnaryOperationType(std::string syntax, evaluation_function_t evaluationFunction)
    : m_syntax(std::move(syntax)),
      m_evaluation_function(std::move(evaluationFunction))
{
}

const SimpleExpressionUnaryOperationType SimpleExpressionUnaryOperationType::OPERATION_NOT(
    "!",
    [](const SimpleExpressionValue& operand) -> SimpleExpressionValue
    {
        return SimpleExpressionValue(!operand.IsTruthy());
    }
);

const SimpleExpressionUnaryOperationType SimpleExpressionUnaryOperationType::OPERATION_BITWISE_NOT(
    "~",
    [](const SimpleExpressionValue& operand) -> SimpleExpressionValue
    {
        if(operand.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(~operand.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionUnaryOperationType SimpleExpressionUnaryOperationType::OPERATION_NEGATIVE(
    "-",
    [](const SimpleExpressionValue& operand) -> SimpleExpressionValue
    {
        if(operand.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(-operand.m_int_value);
        if (operand.m_type == SimpleExpressionValue::Type::DOUBLE)
            return SimpleExpressionValue(-operand.m_double_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionUnaryOperationType* const SimpleExpressionUnaryOperationType::ALL_OPERATION_TYPES[static_cast<int>(SimpleUnaryOperationId::COUNT)]
{
    &OPERATION_NOT,
    &OPERATION_BITWISE_NOT,
    &OPERATION_NEGATIVE,
};

SimpleExpressionUnaryOperation::SimpleExpressionUnaryOperation(const SimpleExpressionUnaryOperationType* operationType, std::unique_ptr<ISimpleExpression> operand)
    : m_operation_type(operationType),
      m_operand(std::move(operand))
{
}

bool SimpleExpressionUnaryOperation::OperandNeedsParenthesis() const
{
    return dynamic_cast<const SimpleExpressionBinaryOperation*>(m_operand.get()) != nullptr;
}

bool SimpleExpressionUnaryOperation::IsStatic()
{
    assert(m_operand);

    return m_operand->IsStatic();
}

SimpleExpressionValue SimpleExpressionUnaryOperation::Evaluate()
{
    return m_operation_type->m_evaluation_function(m_operand->Evaluate());
}
