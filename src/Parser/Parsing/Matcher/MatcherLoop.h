#pragma once

#include "AbstractMatcher.h"
#include "Parsing/IParserValue.h"

#include <memory>

template<typename TokenType> class MatcherLoop final : public AbstractMatcher<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::unique_ptr<AbstractMatcher<TokenType>> m_matcher;

protected:
    MatcherResult<TokenType> CanMatch(ILexer<TokenType>* lexer, const unsigned tokenOffset) override
    {
        auto matchResult = MatcherResult<TokenType>::Match(0);
        auto loopedAtLeastOnce = false;

        while (true)
        {
            auto result = m_matcher->Match(lexer, tokenOffset + matchResult.m_consumed_token_count);

            if (!result.m_matches)
            {
                if (loopedAtLeastOnce)
                    return matchResult;

                return MatcherResult<TokenType>::NoMatch();
            }

            loopedAtLeastOnce = true;
            matchResult.Absorb(std::move(result));
        }
    }

public:
    explicit MatcherLoop(std::unique_ptr<AbstractMatcher<TokenType>> matcher)
        : m_matcher(std::move(matcher))
    {
    }
};
