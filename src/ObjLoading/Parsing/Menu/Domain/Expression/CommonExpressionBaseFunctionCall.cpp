#include "CommonExpressionBaseFunctionCall.h"

using namespace menu;

CommonExpressionBaseFunctionCall::CommonExpressionBaseFunctionCall(std::string functionName, size_t functionIndex)
    : m_function_name(std::move(functionName)),
      m_function_index(functionIndex)
{
}

bool CommonExpressionBaseFunctionCall::Equals(const ISimpleExpression* other) const
{
    const auto otherFunctionCall = dynamic_cast<const CommonExpressionBaseFunctionCall*>(other);

    if (!otherFunctionCall || m_function_name != otherFunctionCall->m_function_name || m_function_index != otherFunctionCall->m_function_index
        || m_args.size() != otherFunctionCall->m_args.size())
    {
        return false;
    }

    for (auto i = 0u; i < m_args.size(); i++)
    {
        const auto* arg = m_args[i].get();
        const auto* otherArg = otherFunctionCall->m_args[i].get();

        if (!arg->Equals(otherArg))
            return false;
    }

    return true;
}

bool CommonExpressionBaseFunctionCall::IsStatic() const
{
    return false;
}

SimpleExpressionValue CommonExpressionBaseFunctionCall::EvaluateStatic() const
{
    return SimpleExpressionValue(0);
}

SimpleExpressionValue CommonExpressionBaseFunctionCall::EvaluateNonStatic(const ISimpleExpressionScopeValues* scopeValues) const
{
    return SimpleExpressionValue(0);
}
