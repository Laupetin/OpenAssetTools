#include "SimpleExpressionBinaryOperation.h"

#include <cassert>
#include <cmath>

SimpleExpressionBinaryOperationType::SimpleExpressionBinaryOperationType(const SimpleBinaryOperationId id, std::string syntax, const SimpleOperationPrecedence precedence,
                                                                         evaluation_function_t evaluationFunction)
    : m_id(id),
      m_syntax(std::move(syntax)),
      m_precedence(precedence),
      m_evaluation_function(std::move(evaluationFunction))
{
}

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_ADD(
    SimpleBinaryOperationId::ADD,
    "+",
    SimpleOperationPrecedence::ADDITION_SUBTRACTION,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value + operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value + static_cast<double>(operand2.m_int_value));
            if (operand2.m_type == SimpleExpressionValue::Type::STRING)
                return SimpleExpressionValue(std::to_string(operand1.m_double_value) + *operand2.m_string_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(static_cast<double>(operand1.m_int_value) + operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value + operand2.m_int_value);
            if (operand2.m_type == SimpleExpressionValue::Type::STRING)
                return SimpleExpressionValue(std::to_string(operand1.m_int_value) + *operand2.m_string_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::STRING)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(*operand1.m_string_value + std::to_string(operand2.m_double_value));
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(*operand1.m_string_value + std::to_string(operand2.m_int_value));
            if (operand2.m_type == SimpleExpressionValue::Type::STRING)
                return SimpleExpressionValue(*operand1.m_string_value + *operand2.m_string_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_SUBTRACT(
    SimpleBinaryOperationId::SUBTRACT,
    "-",
    SimpleOperationPrecedence::ADDITION_SUBTRACTION,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value - operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value - static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(static_cast<double>(operand1.m_int_value) - operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value - operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_MULTIPLY(
    SimpleBinaryOperationId::MULTIPLY,
    "*",
    SimpleOperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value * operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value * static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(static_cast<double>(operand1.m_int_value) * operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value * operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_DIVIDE(
    SimpleBinaryOperationId::DIVIDE,
    "/",
    SimpleOperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value / operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value / static_cast<double>(operand2.m_int_value));
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(static_cast<double>(operand1.m_int_value) / operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value / operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_REMAINDER(
    SimpleBinaryOperationId::REMAINDER,
    "%",
    SimpleOperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::INT && operand2.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(operand1.m_int_value % operand2.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_BITWISE_AND(
    SimpleBinaryOperationId::BITWISE_AND,
    "&",
    SimpleOperationPrecedence::BITWISE_AND,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::INT && operand2.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(operand1.m_int_value & operand2.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_BITWISE_OR(
    SimpleBinaryOperationId::BITWISE_OR,
    "|",
    SimpleOperationPrecedence::BITWISE_OR,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::INT && operand2.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(operand1.m_int_value | operand2.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_SHIFT_LEFT(
    SimpleBinaryOperationId::SHIFT_LEFT,
    "<<",
    SimpleOperationPrecedence::BITWISE_SHIFT,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::INT && operand2.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(operand1.m_int_value << operand2.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_SHIFT_RIGHT(
    SimpleBinaryOperationId::SHIFT_RIGHT,
    ">>",
    SimpleOperationPrecedence::BITWISE_SHIFT,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::INT && operand2.m_type == SimpleExpressionValue::Type::INT)
            return SimpleExpressionValue(operand1.m_int_value >> operand2.m_int_value);

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_GREATER_THAN(
    SimpleBinaryOperationId::GREATER_THAN,
    ">",
    SimpleOperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::STRING || operand2.m_type == SimpleExpressionValue::Type::STRING)
            return SimpleExpressionValue(operand1.IsTruthy() > operand2.IsTruthy());

        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value > operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value > operand2.m_int_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_int_value > operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value > operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_GREATER_EQUAL_THAN(
    SimpleBinaryOperationId::GREATER_EQUAL_THAN,
    ">=",
    SimpleOperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::STRING || operand2.m_type == SimpleExpressionValue::Type::STRING)
            return SimpleExpressionValue(operand1.IsTruthy() >= operand2.IsTruthy());

        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value >= operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value >= operand2.m_int_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_int_value >= operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value >= operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_LESS_THAN(
    SimpleBinaryOperationId::LESS_THAN,
    "<",
    SimpleOperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::STRING || operand2.m_type == SimpleExpressionValue::Type::STRING)
            return SimpleExpressionValue(operand1.IsTruthy() < operand2.IsTruthy());

        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value < operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value < operand2.m_int_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_int_value < operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value < operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_LESS_EQUAL_THAN(
    SimpleBinaryOperationId::LESS_EQUAL_THAN,
    "<=",
    SimpleOperationPrecedence::RELATIONAL_GREATER_LESS_THAN,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::STRING || operand2.m_type == SimpleExpressionValue::Type::STRING)
            return SimpleExpressionValue(operand1.IsTruthy() <= operand2.IsTruthy());

        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_double_value <= operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_double_value <= operand2.m_int_value);
        }
        else if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(operand1.m_int_value <= operand2.m_double_value);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value <= operand2.m_int_value);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_EQUALS(
    SimpleBinaryOperationId::EQUALS,
    "==",
    SimpleOperationPrecedence::RELATIONAL_EQUALS,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) == FP_ZERO);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - static_cast<double>(operand2.m_int_value)) == FP_ZERO);

            return SimpleExpressionValue(0);
        }

        if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) == FP_ZERO);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value == operand2.m_int_value);

            return SimpleExpressionValue(0);
        }

        if (operand1.m_type == SimpleExpressionValue::Type::STRING)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::STRING)
                return SimpleExpressionValue(*operand1.m_string_value == *operand2.m_string_value);

            return SimpleExpressionValue(0);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_NOT_EQUAL(
    SimpleBinaryOperationId::NOT_EQUAL,
    "!=",
    SimpleOperationPrecedence::RELATIONAL_EQUALS,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.m_type == SimpleExpressionValue::Type::DOUBLE)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) != FP_ZERO);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - static_cast<double>(operand2.m_int_value)) != FP_ZERO);

            return SimpleExpressionValue(0);
        }

        if (operand1.m_type == SimpleExpressionValue::Type::INT)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::DOUBLE)
                return SimpleExpressionValue(std::fpclassify(operand1.m_double_value - operand2.m_double_value) != FP_ZERO);
            if (operand2.m_type == SimpleExpressionValue::Type::INT)
                return SimpleExpressionValue(operand1.m_int_value != operand2.m_int_value);

            return SimpleExpressionValue(0);
        }

        if (operand1.m_type == SimpleExpressionValue::Type::STRING)
        {
            if (operand2.m_type == SimpleExpressionValue::Type::STRING)
                return SimpleExpressionValue(*operand1.m_string_value != *operand2.m_string_value);

            return SimpleExpressionValue(0);
        }

        return SimpleExpressionValue(0);
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_AND(
    SimpleBinaryOperationId::AND,
    "&&",
    SimpleOperationPrecedence::LOGICAL_AND,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.IsTruthy())
            return operand2;
        return operand1;
    }
);

const SimpleExpressionBinaryOperationType SimpleExpressionBinaryOperationType::OPERATION_OR(
    SimpleBinaryOperationId::OR,
    "||",
    SimpleOperationPrecedence::LOGICAL_OR,
    [](const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2) -> SimpleExpressionValue
    {
        if (operand1.IsTruthy())
            return operand1;
        return operand2;
    }
);

const SimpleExpressionBinaryOperationType* const SimpleExpressionBinaryOperationType::ALL_OPERATION_TYPES[static_cast<int>(SimpleBinaryOperationId::COUNT)]
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

SimpleExpressionBinaryOperation::SimpleExpressionBinaryOperation(const SimpleExpressionBinaryOperationType* operationType, std::unique_ptr<ISimpleExpression> operand1,
                                                                 std::unique_ptr<ISimpleExpression> operand2)
    : m_operation_type(operationType),
      m_operand1(std::move(operand1)),
      m_operand2(std::move(operand2))
{
}

bool SimpleExpressionBinaryOperation::Operand1NeedsParenthesis() const
{
    const auto* operation = dynamic_cast<const SimpleExpressionBinaryOperation*>(m_operand1.get());
    return operation && operation->m_operation_type->m_precedence > m_operation_type->m_precedence;
}

bool SimpleExpressionBinaryOperation::Operand2NeedsParenthesis() const
{
    const auto* operation = dynamic_cast<const SimpleExpressionBinaryOperation*>(m_operand2.get());
    return operation && operation->m_operation_type->m_precedence > m_operation_type->m_precedence;
}

bool SimpleExpressionBinaryOperation::IsStatic()
{
    assert(m_operand1 && m_operand2);

    return m_operand1->IsStatic() && m_operand2->IsStatic();
}

SimpleExpressionValue SimpleExpressionBinaryOperation::Evaluate()
{
    return m_operation_type->m_evaluation_function(m_operand1->Evaluate(), m_operand2->Evaluate());
}
