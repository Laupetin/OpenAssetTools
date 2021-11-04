#pragma once

#include <iterator>
#include <memory>

#include "Parsing/IParserValue.h"
#include "AbstractMatcher.h"

template <typename TokenType>
class MatcherAnd final : public AbstractMatcher<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::vector<std::unique_ptr<AbstractMatcher<TokenType>>> m_matchers;

protected:
    MatcherResult<TokenType> CanMatch(ILexer<TokenType>* lexer, const unsigned tokenOffset) override
    {
        auto matchResult = MatcherResult<TokenType>::Match(0);

        for (const std::unique_ptr<AbstractMatcher<TokenType>>& matcher : m_matchers)
        {
            MatcherResult<TokenType> result = matcher->Match(lexer, tokenOffset + matchResult.m_consumed_token_count);

            if (!result.m_matches)
                return MatcherResult<TokenType>::NoMatch();
            
            matchResult.Absorb(std::move(result));
        }
        
        return matchResult;
    }

public:
    MatcherAnd(std::initializer_list<Movable<std::unique_ptr<AbstractMatcher<TokenType>>>> matchers)
        : m_matchers(std::make_move_iterator(matchers.begin()), std::make_move_iterator(matchers.end()))
    {
    }

    explicit MatcherAnd(std::vector<std::unique_ptr<AbstractMatcher<TokenType>>> matchers)
        : m_matchers(std::move(matchers))
    {
    }
};
