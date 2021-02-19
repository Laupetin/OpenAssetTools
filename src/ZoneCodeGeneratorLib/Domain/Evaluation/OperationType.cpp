#include "OperationType.h"

OperationType::OperationType(std::string syntax, const OperationPrecedence precedence, std::function<int(int, int)> evaluationFunction)
    : m_syntax(std::move(syntax)),
      m_precedence(precedence),
      m_evaluation_function(std::move(evaluationFunction))
{
}

const OperationType* const OperationType::OPERATION_ADD
    = new OperationType("+", OperationPrecedence::ADDITION_SUBTRACTION, [](auto op1, auto op2)
    {
        return op1 + op2;
    });

const OperationType* const OperationType::OPERATION_SUBTRACT
    = new OperationType("-", OperationPrecedence::ADDITION_SUBTRACTION, [](auto op1, auto op2)
    {
        return op1 - op2;
    });

const OperationType* const OperationType::OPERATION_MULTIPLY
    = new OperationType("*", OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER, [](auto op1, auto op2)
    {
        return op1 * op2;
    });

const OperationType* const OperationType::OPERATION_DIVIDE
    = new OperationType("/", OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER, [](auto op1, auto op2)
    {
        return op1 / op2;
    });

const OperationType* const OperationType::OPERATION_REMAINDER
    = new OperationType("%", OperationPrecedence::MULTIPLICATION_DIVISION_REMAINDER, [](auto op1, auto op2)
    {
        return op1 % op2;
    });

const OperationType* const OperationType::OPERATION_BITWISE_AND
    = new OperationType("&", OperationPrecedence::BITWISE_AND, [](auto op1, auto op2)
    {
        return op1 & op2;
    });

const OperationType* const OperationType::OPERATION_BITWISE_XOR
    = new OperationType("^", OperationPrecedence::BITWISE_XOR, [](auto op1, auto op2)
    {
        return op1 ^ op2;
    });

const OperationType* const OperationType::OPERATION_BITWISE_OR
    = new OperationType("|", OperationPrecedence::BITWISE_OR, [](auto op1, auto op2)
    {
        return op1 | op2;
    });

const OperationType* const OperationType::OPERATION_SHIFT_LEFT
    = new OperationType("<<", OperationPrecedence::BITWISE_SHIFT, [](auto op1, auto op2)
    {
        return op1 << op2;
    });

const OperationType* const OperationType::OPERATION_SHIFT_RIGHT
    = new OperationType(">>", OperationPrecedence::BITWISE_SHIFT, [](auto op1, auto op2)
    {
        return op1 >> op2;
    });

const OperationType* const OperationType::OPERATION_GREATER_THAN
    = new OperationType(">", OperationPrecedence::RELATIONAL_GREATER_LESS_THAN, [](auto op1, auto op2)
    {
        return op1 > op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_GREATER_EQUAL_THAN
    = new OperationType(">=", OperationPrecedence::RELATIONAL_GREATER_LESS_THAN, [](auto op1, auto op2)
    {
        return op1 >= op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_LESS_THAN
    = new OperationType("<", OperationPrecedence::RELATIONAL_GREATER_LESS_THAN, [](auto op1, auto op2)
    {
        return op1 < op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_LESS_EQUAL_THAN
    = new OperationType("<=", OperationPrecedence::RELATIONAL_GREATER_LESS_THAN, [](auto op1, auto op2)
    {
        return op1 <= op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_EQUALS
    = new OperationType("==", OperationPrecedence::RELATIONAL_EQUALS, [](auto op1, auto op2)
    {
        return op1 == op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_NOT_EQUAL
    = new OperationType("!=", OperationPrecedence::RELATIONAL_EQUALS, [](auto op1, auto op2)
    {
        return op1 != op2 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_AND
    = new OperationType("&&", OperationPrecedence::LOGICAL_AND, [](auto op1, auto op2)
    {
        return op1 > 0 && op2 > 0 ? 1 : 0;
    });

const OperationType* const OperationType::OPERATION_OR
    = new OperationType("||", OperationPrecedence::LOGICAL_OR, [](auto op1, auto op2)
    {
        return op1 > 0 || op2 > 0 ? 1 : 0;
    });


const OperationType* const OperationType::ALL_OPERATION_TYPES[]
{
    OPERATION_ADD,
    OPERATION_SUBTRACT,
    OPERATION_MULTIPLY,
    OPERATION_DIVIDE,
    OPERATION_REMAINDER,
    OPERATION_BITWISE_AND,
    OPERATION_BITWISE_XOR,
    OPERATION_BITWISE_OR,
    OPERATION_SHIFT_LEFT,
    OPERATION_SHIFT_RIGHT,
    OPERATION_GREATER_THAN,
    OPERATION_GREATER_EQUAL_THAN,
    OPERATION_LESS_THAN,
    OPERATION_LESS_EQUAL_THAN,
    OPERATION_EQUALS,
    OPERATION_NOT_EQUAL,
    OPERATION_AND,
    OPERATION_OR
};
