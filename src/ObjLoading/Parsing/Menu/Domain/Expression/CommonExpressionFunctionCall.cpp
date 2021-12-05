#include "CommonExpressionFunctionCall.h"

using namespace menu;

CommonExpressionFunctionCall::CommonExpressionFunctionCall(std::string functionName)
    : m_function_name(std::move(functionName))
{
}

bool CommonExpressionFunctionCall::IsStatic() const
{
    return false;
}

SimpleExpressionValue CommonExpressionFunctionCall::Evaluate() const
{
    return SimpleExpressionValue(0);
}
