#pragma once

#include <cassert>
#include <deque>

#include "ILexer.h"
#include "IParserLineStream.h"

template <typename TokenType>
class AbstractLexer : public ILexer
{
    std::deque<TokenType> m_token_cache;
    IParserLineStream* const m_stream;

protected:
    explicit AbstractLexer(IParserLineStream* stream)
        : m_stream(stream)
    {
    }

    virtual TokenType GetNextToken() = 0;

public:
    const TokenType& GetToken(int index)
    {
        assert(index >= 0);
        while (index <= m_token_cache.size())
            m_token_cache.emplace_back(std::move(GetNextToken()));

        return m_token_cache[index];
    }

    void PopTokens(int amount) override
    {
        m_token_cache.erase(m_token_cache.begin(), m_token_cache.begin() + amount);
    }
};
