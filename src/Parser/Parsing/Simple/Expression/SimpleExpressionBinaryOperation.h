#pragma once

#include "ISimpleExpression.h"
#include "SimpleExpressionValue.h"

#include <functional>
#include <memory>
#include <string>

// https://en.cppreference.com/w/cpp/language/operator_precedence
enum class SimpleOperationPrecedence
{
    MULTIPLICATION_DIVISION_REMAINDER = 1,
    ADDITION_SUBTRACTION = 2,
    BITWISE_SHIFT = 3,
    RELATIONAL_GREATER_LESS_THAN = 4,
    RELATIONAL_EQUALS = 5,
    BITWISE_AND = 6,
    BITWISE_OR = 8,
    LOGICAL_AND = 9,
    LOGICAL_OR = 10
};

enum class SimpleBinaryOperationId
{
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    REMAINDER,
    BITWISE_AND,
    BITWISE_OR,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    GREATER_THAN,
    GREATER_EQUAL_THAN,
    LESS_THAN,
    LESS_EQUAL_THAN,
    EQUALS,
    NOT_EQUAL,
    AND,
    OR,

    COUNT
};

class SimpleExpressionBinaryOperationType
{
public:
    using evaluation_function_t = std::function<SimpleExpressionValue(const SimpleExpressionValue& operand1, const SimpleExpressionValue& operand2)>;

    SimpleBinaryOperationId m_id;
    std::string m_syntax;
    SimpleOperationPrecedence m_precedence;
    evaluation_function_t m_evaluation_function;

private:
    SimpleExpressionBinaryOperationType(SimpleBinaryOperationId id,
                                        std::string syntax,
                                        SimpleOperationPrecedence precedence,
                                        evaluation_function_t evaluationFunction);

public:
    static const SimpleExpressionBinaryOperationType OPERATION_ADD;
    static const SimpleExpressionBinaryOperationType OPERATION_SUBTRACT;
    static const SimpleExpressionBinaryOperationType OPERATION_MULTIPLY;
    static const SimpleExpressionBinaryOperationType OPERATION_DIVIDE;
    static const SimpleExpressionBinaryOperationType OPERATION_REMAINDER;
    static const SimpleExpressionBinaryOperationType OPERATION_BITWISE_AND;
    static const SimpleExpressionBinaryOperationType OPERATION_BITWISE_OR;
    static const SimpleExpressionBinaryOperationType OPERATION_SHIFT_LEFT;
    static const SimpleExpressionBinaryOperationType OPERATION_SHIFT_RIGHT;
    static const SimpleExpressionBinaryOperationType OPERATION_GREATER_THAN;
    static const SimpleExpressionBinaryOperationType OPERATION_GREATER_EQUAL_THAN;
    static const SimpleExpressionBinaryOperationType OPERATION_LESS_THAN;
    static const SimpleExpressionBinaryOperationType OPERATION_LESS_EQUAL_THAN;
    static const SimpleExpressionBinaryOperationType OPERATION_EQUALS;
    static const SimpleExpressionBinaryOperationType OPERATION_NOT_EQUAL;
    static const SimpleExpressionBinaryOperationType OPERATION_AND;
    static const SimpleExpressionBinaryOperationType OPERATION_OR;

    static const SimpleExpressionBinaryOperationType* const ALL_OPERATION_TYPES[static_cast<int>(SimpleBinaryOperationId::COUNT)];
};

class SimpleExpressionBinaryOperation final : public ISimpleExpression
{
public:
    const SimpleExpressionBinaryOperationType* m_operation_type;
    std::unique_ptr<ISimpleExpression> m_operand1;
    std::unique_ptr<ISimpleExpression> m_operand2;

    SimpleExpressionBinaryOperation(const SimpleExpressionBinaryOperationType* operationType,
                                    std::unique_ptr<ISimpleExpression> operand1,
                                    std::unique_ptr<ISimpleExpression> operand2);

    _NODISCARD bool Operand1NeedsParenthesis() const;
    _NODISCARD bool Operand2NeedsParenthesis() const;

    _NODISCARD bool Equals(const ISimpleExpression* other) const override;
    _NODISCARD bool IsStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateStatic() const override;
    _NODISCARD SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
};
