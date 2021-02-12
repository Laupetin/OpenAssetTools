#pragma once

#include <iterator>
#include <memory>

#include "Parsing/IParserValue.h"
#include "AbstractMatcher.h"

template <typename TokenType>
class MatcherOptional final : public AbstractMatcher<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::unique_ptr<AbstractMatcher<TokenType>> m_matcher;

protected:
    MatcherResult<TokenType> CanMatch(AbstractLexer<TokenType>* lexer, unsigned tokenOffset) override
    {
        auto result = m_matcher->Match(lexer, tokenOffset);

        if (result.m_matches)
            return result;

        return MatcherResult<TokenType>::Match(0);
    }

public:
    explicit MatcherOptional(std::unique_ptr<AbstractMatcher<TokenType>> matcher)
        : m_matcher(std::move(matcher))
    {
    }
};
