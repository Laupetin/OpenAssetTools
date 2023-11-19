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

        _NODISCARD bool Equals(const ISimpleExpression* other) const override;
        _NODISCARD bool IsStatic() const override;
        _NODISCARD SimpleExpressionValue EvaluateStatic() const override;
        _NODISCARD SimpleExpressionValue EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const override;
    };
} // namespace menu
