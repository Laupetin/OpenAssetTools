#pragma once

#include <string>
#include <memory>
#include <functional>

#include "ICommonExpression.h"
#include "CommonExpressionValue.h"

namespace menu
{
    class CommonExpressionUnaryOperationType
    {
    public:
        using evaluation_function_t = std::function<CommonExpressionValue(const CommonExpressionValue& operand)>;

        std::string m_syntax;
        evaluation_function_t m_evaluation_function;

    private:
        CommonExpressionUnaryOperationType(std::string syntax, evaluation_function_t evaluationFunction);

        static const CommonExpressionUnaryOperationType OPERATION_NOT;
        static const CommonExpressionUnaryOperationType OPERATION_BITWISE_NOT;

        static const CommonExpressionUnaryOperationType* const ALL_OPERATION_TYPES[];
    };

    class CommonExpressionUnaryOperation final : public ICommonExpression
    {
    public:
        std::unique_ptr<ICommonExpression> m_operand;
        const CommonExpressionUnaryOperationType* m_operation_type;

        CommonExpressionUnaryOperation(std::unique_ptr<ICommonExpression> operand,
            const CommonExpressionUnaryOperationType* operationType);

        _NODISCARD bool OperandNeedsParenthesis() const;

        bool IsStatic() override;
        CommonExpressionValue Evaluate() override;
    };
}
