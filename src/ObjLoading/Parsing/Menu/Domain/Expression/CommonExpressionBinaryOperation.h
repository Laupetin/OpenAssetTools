#pragma once

#include <string>
#include <memory>
#include <functional>

#include "ICommonExpression.h"
#include "CommonExpressionValue.h"

namespace menu
{
    // https://en.cppreference.com/w/cpp/language/operator_precedence
    enum class OperationPrecedence
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

    enum class BinaryOperationId
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

    class CommonExpressionBinaryOperationType
    {
    public:
        using evaluation_function_t = std::function<CommonExpressionValue(const CommonExpressionValue& operand1, const CommonExpressionValue& operand2)>;

        std::string m_syntax;
        OperationPrecedence m_precedence;
        evaluation_function_t m_evaluation_function;

    private:
        CommonExpressionBinaryOperationType(std::string syntax, OperationPrecedence precedence, evaluation_function_t evaluationFunction);

    public:
        static const CommonExpressionBinaryOperationType OPERATION_ADD;
        static const CommonExpressionBinaryOperationType OPERATION_SUBTRACT;
        static const CommonExpressionBinaryOperationType OPERATION_MULTIPLY;
        static const CommonExpressionBinaryOperationType OPERATION_DIVIDE;
        static const CommonExpressionBinaryOperationType OPERATION_REMAINDER;
        static const CommonExpressionBinaryOperationType OPERATION_BITWISE_AND;
        static const CommonExpressionBinaryOperationType OPERATION_BITWISE_OR;
        static const CommonExpressionBinaryOperationType OPERATION_SHIFT_LEFT;
        static const CommonExpressionBinaryOperationType OPERATION_SHIFT_RIGHT;
        static const CommonExpressionBinaryOperationType OPERATION_GREATER_THAN;
        static const CommonExpressionBinaryOperationType OPERATION_GREATER_EQUAL_THAN;
        static const CommonExpressionBinaryOperationType OPERATION_LESS_THAN;
        static const CommonExpressionBinaryOperationType OPERATION_LESS_EQUAL_THAN;
        static const CommonExpressionBinaryOperationType OPERATION_EQUALS;
        static const CommonExpressionBinaryOperationType OPERATION_NOT_EQUAL;
        static const CommonExpressionBinaryOperationType OPERATION_AND;
        static const CommonExpressionBinaryOperationType OPERATION_OR;

        static const CommonExpressionBinaryOperationType* const ALL_OPERATION_TYPES[static_cast<int>(BinaryOperationId::COUNT)];
    };

    class CommonExpressionBinaryOperation final : public ICommonExpression
    {
    public:
        const CommonExpressionBinaryOperationType* m_operation_type;
        std::unique_ptr<ICommonExpression> m_operand1;
        std::unique_ptr<ICommonExpression> m_operand2;

        CommonExpressionBinaryOperation(const CommonExpressionBinaryOperationType* operationType,
                                        std::unique_ptr<ICommonExpression> operand1,
                                        std::unique_ptr<ICommonExpression> operand2);

        _NODISCARD bool Operand1NeedsParenthesis() const;
        _NODISCARD bool Operand2NeedsParenthesis() const;

        bool IsStatic() override;
        CommonExpressionValue Evaluate() override;
    };
}
