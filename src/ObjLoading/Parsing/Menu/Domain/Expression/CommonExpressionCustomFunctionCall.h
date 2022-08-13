#pragma once

#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonExpressionCustomFunctionCall final : public ISimpleExpression
    {
    public:
        std::string m_function_name;

        explicit CommonExpressionCustomFunctionCall(std::string functionName);

        _NODISCARD bool Equals(const ISimpleExpression* other) const override;
        _NODISCARD bool IsStatic() const override;
        _NODISCARD SimpleExpressionValue EvaluateStatic() const override;
        _NODISCARD SimpleExpressionValue EvaluateNonStatic(ISimpleExpressionScopeValues* scopeValues) const override;
    };
}
