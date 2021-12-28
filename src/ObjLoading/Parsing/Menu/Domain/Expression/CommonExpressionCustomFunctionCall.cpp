#include "CommonExpressionCustomFunctionCall.h"

using namespace menu;

CommonExpressionCustomFunctionCall::CommonExpressionCustomFunctionCall(std::string functionName)
    : m_function_name(std::move(functionName))
{
}

bool CommonExpressionCustomFunctionCall::Equals(const ISimpleExpression* other) const
{
    const auto otherFunctionCall = dynamic_cast<const CommonExpressionCustomFunctionCall*>(other);
    return otherFunctionCall && m_function_name == otherFunctionCall->m_function_name;
}

bool CommonExpressionCustomFunctionCall::IsStatic() const
{
    return false;
}

SimpleExpressionValue CommonExpressionCustomFunctionCall::Evaluate() const
{
    return SimpleExpressionValue(0);
}
