#include "CommonExpressionFunctionCall.h"

using namespace menu;

CommonExpressionFunctionCall::CommonExpressionFunctionCall(std::string functionName)
    : m_function_name(std::move(functionName))
{
}

bool CommonExpressionFunctionCall::Equals(const ISimpleExpression* other) const
{
    const auto otherFunctionCall = dynamic_cast<const CommonExpressionFunctionCall*>(other);

    if (!otherFunctionCall 
        || m_function_name != otherFunctionCall->m_function_name
        || m_args.size() != otherFunctionCall->m_args.size())
    {
        return false;
    }

    for(auto i = 0u; i < m_args.size(); i++)
    {
        const auto* arg = m_args[i].get();
        const auto* otherArg = otherFunctionCall->m_args[i].get();

        if (!arg->Equals(otherArg))
            return false;
    }

    return true;
}

bool CommonExpressionFunctionCall::IsStatic() const
{
    return false;
}

SimpleExpressionValue CommonExpressionFunctionCall::Evaluate() const
{
    return SimpleExpressionValue(0);
}
