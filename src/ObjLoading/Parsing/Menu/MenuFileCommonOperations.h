#pragma once
#include "MenuFileParserState.h"
#include "Parsing/TokenPos.h"
#include "Parsing/Simple/Expression/ISimpleExpression.h"

namespace menu
{
    class MenuFileCommonOperations
    {
    public:
        static void EnsureIsNumericExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression);
        static void EnsureIsStringExpression(const MenuFileParserState* state, const TokenPos& pos, const ISimpleExpression& expression);
    };
}
