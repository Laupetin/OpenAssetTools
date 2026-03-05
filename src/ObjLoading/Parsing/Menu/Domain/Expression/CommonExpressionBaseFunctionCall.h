#pragma once

#include "Parsing/Simple/Expression/ISimpleExpression.h"

#include <vector>

namespace menu
{
    class CommonExpressionBaseFunctionCall final : public ISimpleExpression
    {
    public:
        std::string m_function_name;
        size_t m_function_index;
        std::vector<std::unique_ptr<ISimpleExpression>> m_args;

        CommonExpressionBaseFunctionCall(std::string functionName, size_t functionIndex);

        [[nodiscard]] bool Equals(const ISimpleExpression* other) const override;
        [[nodiscard]] bool IsStatic() const override;
        [[nodiscard]] SimpleExpressionValue EvaluateStatic() const override;
        [[nodiscard]] SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
    };
} // namespace menu
