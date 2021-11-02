#pragma once
#include <vector>

#include "ICommonExpression.h"

namespace menu
{
    class CommonExpressionFunctionCall final : public ICommonExpression
    {
    public:
        std::string m_function_name;
        std::vector<std::unique_ptr<ICommonExpression>> m_args;

        explicit CommonExpressionFunctionCall(std::string functionName);

        bool IsStatic() override;
        CommonExpressionValue Evaluate() override;
    };
}
