#pragma once

#include "AbstractMatcher.h"
#include "Parsing/IParserValue.h"

#include <memory>

template<typename TokenType> class MatcherFalse final : public AbstractMatcher<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

protected:
    MatcherResult<TokenType> CanMatch(ILexer<TokenType>* lexer, unsigned tokenOffset) override
    {
        return MatcherResult<TokenType>::NoMatch();
    }

public:
    MatcherFalse() = default;
};
