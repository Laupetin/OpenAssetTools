#include "MenuFileCommonOperations.h"

#include "Parsing/ParsingException.h"

using namespace menu;

void MenuFileCommonOperations::EnsureIsNumericExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression)
{
    if (!state->m_permissive_mode && expression.IsStatic())
    {
        const auto staticValue = expression.EvaluateStatic();

        if (staticValue.m_type != SimpleExpressionValue::Type::INT && staticValue.m_type != SimpleExpressionValue::Type::DOUBLE)
            throw ParsingException(pos, "Expression is expected to be numeric. Use permissive mode to compile anyway.");
    }
}

void MenuFileCommonOperations::EnsureIsStringExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression)
{
    if (!state->m_permissive_mode && expression.IsStatic())
    {
        const auto staticValue = expression.EvaluateStatic();

        if (staticValue.m_type != SimpleExpressionValue::Type::STRING)
            throw ParsingException(pos, "Expression is expected to be string. Use permissive mode to compile anyway.");
    }
}
