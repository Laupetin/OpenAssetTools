#pragma once
#include "MenuFileParserState.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"
#include "Parsing/TokenPos.h"

namespace menu
{
    class MenuFileCommonOperations
    {
    public:
        static void EnsureIsNumericExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression);
        static void EnsureIsStringExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression);
    };
} // namespace menu
