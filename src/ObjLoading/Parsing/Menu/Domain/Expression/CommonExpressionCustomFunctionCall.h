#pragma once

#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class CommonExpressionCustomFunctionCall final : public ISimpleExpression
    {
    public:
        std::string m_function_name;

        explicit CommonExpressionCustomFunctionCall(std::string functionName);

        [[nodiscard]] bool Equals(const ISimpleExpression* other) const override;
        [[nodiscard]] bool IsStatic() const override;
        [[nodiscard]] SimpleExpressionValue EvaluateStatic() const override;
        [[nodiscard]] SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
    };
} // namespace menu
