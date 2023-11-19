#pragma once

#include <functional>
#include <string>

// https://en.cppreference.com/w/cpp/language/operator_precedence
enum class OperationPrecedence
{
    MULTIPLICATION_DIVISION_REMAINDER = 1,
    ADDITION_SUBTRACTION = 2,
    BITWISE_SHIFT = 3,
    RELATIONAL_GREATER_LESS_THAN = 4,
    RELATIONAL_EQUALS = 5,
    BITWISE_AND = 6,
    BITWISE_XOR = 7,
    BITWISE_OR = 8,
    LOGICAL_AND = 9,
    LOGICAL_OR = 10
};

class OperationType
{
public:
    std::string m_syntax;
    OperationPrecedence m_precedence;
    std::function<int(int operand1, int operand2)> m_evaluation_function;

private:
    OperationType(std::string syntax, OperationPrecedence precedence, std::function<int(int, int)> evaluationFunction);

public:
    static const OperationType* const OPERATION_ADD;
    static const OperationType* const OPERATION_SUBTRACT;
    static const OperationType* const OPERATION_MULTIPLY;
    static const OperationType* const OPERATION_DIVIDE;
    static const OperationType* const OPERATION_REMAINDER;
    static const OperationType* const OPERATION_BITWISE_AND;
    static const OperationType* const OPERATION_BITWISE_XOR;
    static const OperationType* const OPERATION_BITWISE_OR;
    static const OperationType* const OPERATION_SHIFT_LEFT;
    static const OperationType* const OPERATION_SHIFT_RIGHT;
    static const OperationType* const OPERATION_GREATER_THAN;
    static const OperationType* const OPERATION_GREATER_EQUAL_THAN;
    static const OperationType* const OPERATION_LESS_THAN;
    static const OperationType* const OPERATION_LESS_EQUAL_THAN;
    static const OperationType* const OPERATION_EQUALS;
    static const OperationType* const OPERATION_NOT_EQUAL;
    static const OperationType* const OPERATION_AND;
    static const OperationType* const OPERATION_OR;

    static const OperationType* const ALL_OPERATION_TYPES[];
};
