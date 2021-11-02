#include "CommonExpressionUnaryOperation.h"

#include <cassert>

#include "CommonExpressionBinaryOperation.h"

using namespace menu;

CommonExpressionUnaryOperationType::CommonExpressionUnaryOperationType(std::string syntax, evaluation_function_t evaluationFunction)
    : m_syntax(std::move(syntax)),
      m_evaluation_function(std::move(evaluationFunction))
{
}

const CommonExpressionUnaryOperationType CommonExpressionUnaryOperationType::OPERATION_NOT(
    "!",
    [](const CommonExpressionValue& operand) -> CommonExpressionValue
    {
        return CommonExpressionValue(!operand.IsTruthy());
    }
);

const CommonExpressionUnaryOperationType CommonExpressionUnaryOperationType::OPERATION_BITWISE_NOT(
    "~",
    [](const CommonExpressionValue& operand) -> CommonExpressionValue
    {
        if(operand.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(~operand.m_int_value);

        return CommonExpressionValue(0);
    }
);

CommonExpressionUnaryOperation::CommonExpressionUnaryOperation(std::unique_ptr<ICommonExpression> operand, const CommonExpressionUnaryOperationType* operationType)
    : m_operand(std::move(operand)),
      m_operation_type(operationType)
{
}

bool CommonExpressionUnaryOperation::OperandNeedsParenthesis() const
{
    return dynamic_cast<const CommonExpressionBinaryOperation*>(m_operand.get()) != nullptr;
}

bool CommonExpressionUnaryOperation::IsStatic()
{
    assert(m_operand);

    return m_operand->IsStatic();
}

CommonExpressionValue CommonExpressionUnaryOperation::Evaluate()
{
    return m_operation_type->m_evaluation_function(m_operand->Evaluate());
}
