#include "CommonExpressionBinaryOperation.h"

#include <cassert>
#include <cmath>

using namespace menu;

CommonExpressionBinaryOperationType::CommonExpressionBinaryOperationType(std::string syntax, const OperationPrecedence precedence, evaluation_function_t evaluationFunction)
    : m_syntax(std::move(syntax)),
      m_precedence(precedence),
      m_evaluation_function(std::move(evaluationFunction))
{
}

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_ADD(
    "+",
    OperationPrecedence::ADDITION_SUBTRACTION,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value + operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value + static_cast<double>(operand2.m_int_value));
            if (operand2.m_type == CommonExpressionValue::Type::STRING)
                return CommonExpressionValue(std::to_string(operand1.m_double_value) + *operand2.m_string_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(static_cast<double>(operand1.m_int_value) + operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value + operand2.m_int_value);
            if (operand2.m_type == CommonExpressionValue::Type::STRING)
                return CommonExpressionValue(std::to_string(operand1.m_int_value) + *operand2.m_string_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::STRING)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(*operand1.m_string_value + std::to_string(operand2.m_double_value));
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(*operand1.m_string_value + std::to_string(operand2.m_int_value));
            if (operand2.m_type == CommonExpressionValue::Type::STRING)
                return CommonExpressionValue(*operand1.m_string_value + *operand2.m_string_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_SUBTRACT(
    "-",
    OperationPrecedence::ADDITION_SUBTRACTION,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value - operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value - static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(static_cast<double>(operand1.m_int_value) - operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value - operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_MULTIPLY(
    "*",
    OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value * operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value * static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(static_cast<double>(operand1.m_int_value) * operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value * operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_DIVIDE(
    "/",
    OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value / operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value / static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(static_cast<double>(operand1.m_int_value) / operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value / operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_REMAINDER(
    "%",
    OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::INT && operand2.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(operand1.m_int_value % operand2.m_int_value);

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_BITWISE_AND(
    "&",
    OperationPrecedence::BITWISE_AND,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::INT && operand2.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(operand1.m_int_value & operand2.m_int_value);

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_BITWISE_OR(
    "|",
    OperationPrecedence::BITWISE_OR,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::INT && operand2.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(operand1.m_int_value | operand2.m_int_value);

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_SHIFT_LEFT(
    "<<",
    OperationPrecedence::BITWISE_SHIFT,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::INT && operand2.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(operand1.m_int_value << operand2.m_int_value);

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_SHIFT_RIGHT(
    ">>",
    OperationPrecedence::BITWISE_SHIFT,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::INT && operand2.m_type == CommonExpressionValue::Type::INT)
            return CommonExpressionValue(operand1.m_int_value >> operand2.m_int_value);

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_GREATER_THAN(
    ">",
    OperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::STRING || operand2.m_type == CommonExpressionValue::Type::STRING)
            return CommonExpressionValue(operand1.IsTruthy() > operand2.IsTruthy());

        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value > operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value > operand2.m_int_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_int_value > operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value > operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_GREATER_EQUAL_THAN(
    ">=",
    OperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::STRING || operand2.m_type == CommonExpressionValue::Type::STRING)
            return CommonExpressionValue(operand1.IsTruthy() >= operand2.IsTruthy());

        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value >= operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value >= operand2.m_int_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_int_value >= operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value >= operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_LESS_THAN(
    "<",
    OperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::STRING || operand2.m_type == CommonExpressionValue::Type::STRING)
            return CommonExpressionValue(operand1.IsTruthy() < operand2.IsTruthy());

        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value < operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value < operand2.m_int_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_int_value < operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value < operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_LESS_EQUAL_THAN(
    "<=",
    OperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::STRING || operand2.m_type == CommonExpressionValue::Type::STRING)
            return CommonExpressionValue(operand1.IsTruthy() <= operand2.IsTruthy());

        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_double_value <= operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_double_value <= operand2.m_int_value);
        }
        else if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(operand1.m_int_value <= operand2.m_double_value);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value <= operand2.m_int_value);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_EQUALS(
    "==",
    OperationPrecedence::RELATIONAL_EQUALS,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) == FP_ZERO);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - static_cast<double>(operand2.m_int_value)) == FP_ZERO);

            return CommonExpressionValue(0);
        }

        if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) == FP_ZERO);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value == operand2.m_int_value);

            return CommonExpressionValue(0);
        }

        if (operand1.m_type == CommonExpressionValue::Type::STRING)
        {
            if (operand2.m_type == CommonExpressionValue::Type::STRING)
                return CommonExpressionValue(*operand1.m_string_value == *operand2.m_string_value);

            return CommonExpressionValue(0);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_NOT_EQUAL(
    "!=",
    OperationPrecedence::RELATIONAL_EQUALS,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.m_type == CommonExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) != FP_ZERO);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - static_cast<double>(operand2.m_int_value)) != FP_ZERO);

            return CommonExpressionValue(0);
        }

        if (operand1.m_type == CommonExpressionValue::Type::INT)
        {
            if (operand2.m_type == CommonExpressionValue::Type::DOUBLE)
                return CommonExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) != FP_ZERO);
            if (operand2.m_type == CommonExpressionValue::Type::INT)
                return CommonExpressionValue(operand1.m_int_value != operand2.m_int_value);

            return CommonExpressionValue(0);
        }

        if (operand1.m_type == CommonExpressionValue::Type::STRING)
        {
            if (operand2.m_type == CommonExpressionValue::Type::STRING)
                return CommonExpressionValue(*operand1.m_string_value != *operand2.m_string_value);

            return CommonExpressionValue(0);
        }

        return CommonExpressionValue(0);
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_AND(
    "&&",
    OperationPrecedence::LOGICAL_AND,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.IsTruthy())
            return operand2;
        return operand1;
    }
);

const CommonExpressionBinaryOperationType CommonExpressionBinaryOperationType::OPERATION_OR(
    "||",
    OperationPrecedence::LOGICAL_OR,
    [](const CommonExpressionValue& operand1, const CommonExpressionValue& operand2) -> CommonExpressionValue
    {
        if (operand1.IsTruthy())
            return operand1;
        return operand2;
    }
);

const CommonExpressionBinaryOperationType* const CommonExpressionBinaryOperationType::ALL_OPERATION_TYPES[static_cast<int>(BinaryOperationId::COUNT)]
{
    &OPERATION_ADD,
    &OPERATION_SUBTRACT,
    &OPERATION_MULTIPLY,
    &OPERATION_DIVIDE,
    &OPERATION_REMAINDER,
    &OPERATION_BITWISE_AND,
    &OPERATION_BITWISE_OR,
    &OPERATION_SHIFT_LEFT,
    &OPERATION_SHIFT_RIGHT,
    &OPERATION_GREATER_THAN,
    &OPERATION_GREATER_EQUAL_THAN,
    &OPERATION_LESS_THAN,
    &OPERATION_LESS_EQUAL_THAN,
    &OPERATION_EQUALS,
    &OPERATION_NOT_EQUAL,
    &OPERATION_AND,
    &OPERATION_OR
};

CommonExpressionBinaryOperation::CommonExpressionBinaryOperation(const CommonExpressionBinaryOperationType* operationType, std::unique_ptr<ICommonExpression> operand1,
                                                                 std::unique_ptr<ICommonExpression> operand2)
    : m_operation_type(operationType),
      m_operand1(std::move(operand1)),
      m_operand2(std::move(operand2))
{
}

bool CommonExpressionBinaryOperation::Operand1NeedsParenthesis() const
{
    const auto* operation = dynamic_cast<const CommonExpressionBinaryOperation*>(m_operand1.get());
    return operation && operation->m_operation_type->m_precedence > m_operation_type->m_precedence;
}

bool CommonExpressionBinaryOperation::Operand2NeedsParenthesis() const
{
    const auto* operation = dynamic_cast<const CommonExpressionBinaryOperation*>(m_operand2.get());
    return operation && operation->m_operation_type->m_precedence > m_operation_type->m_precedence;
}

bool CommonExpressionBinaryOperation::IsStatic()
{
    assert(m_operand1 && m_operand2);

    return m_operand1->IsStatic() && m_operand2->IsStatic();
}

CommonExpressionValue CommonExpressionBinaryOperation::Evaluate()
{
    return m_operation_type->m_evaluation_function(m_operand1->Evaluate(), m_operand2->Evaluate());
}
