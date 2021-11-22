#pragma once
#include <vector>

#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonExpressionFunctionCall final : public ISimpleExpression
    {
    public:
        std::string m_function_name;
        std::vector<std::unique_ptr<ISimpleExpression>> m_args;

        explicit CommonExpressionFunctionCall(std::string functionName);

        bool IsStatic() override;
        SimpleExpressionValue Evaluate() override;
    };
}
