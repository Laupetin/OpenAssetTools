#include "CommonExpressionFunctionCall.h"

using namespace menu;

CommonExpressionFunctionCall::CommonExpressionFunctionCall(std::string functionName)
    : m_function_name(std::move(functionName))
{
}

bool CommonExpressionFunctionCall::IsStatic()
{
    return false;
}

CommonExpressionValue CommonExpressionFunctionCall::Evaluate()
{
    return CommonExpressionValue(0);
}
