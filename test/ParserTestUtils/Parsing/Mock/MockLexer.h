#pragma once

#include <vector>
#include <iterator>

#include "Utils/ClassUtils.h"
#include "Parsing/ILexer.h"

template <typename TokenType>
class MockLexer final : public ILexer<TokenType>
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::vector<TokenType> m_tokens;
    TokenType m_eof;
    int m_pop_count;

public:
    MockLexer(std::initializer_list<Movable<TokenType>> tokens, TokenType eof)
        : m_tokens(std::make_move_iterator(tokens.begin()), std::make_move_iterator(tokens.end())),
        m_eof(std::move(eof)),
        m_pop_count(0)
    {
    }

    MockLexer(std::vector<TokenType> tokens, TokenType eof)
        : m_tokens(std::move(tokens)),
        m_eof(std::move(eof)),
        m_pop_count(0)
    {
    }

    ~MockLexer() override = default;
    MockLexer(const MockLexer& other) = delete;
    MockLexer(MockLexer&& other) noexcept = default;
    MockLexer& operator=(const MockLexer& other) = delete;
    MockLexer& operator=(MockLexer&& other) noexcept = default;

    const TokenType& GetToken(const unsigned index) override
    {
        if (index < m_tokens.size())
            return m_tokens[index];

        return m_eof;
    }

    void PopTokens(const int amount) override
    {
        m_pop_count += amount;
    }

    bool IsEof() override
    {
        return !m_tokens.empty();
    }

    const TokenPos& GetPos() override
    {
        return !m_tokens.empty() ? m_tokens[0].GetPos() : m_eof.GetPos();
    }

    _NODISCARD int GetPopCount() const
    {
        return m_pop_count;
    }

    _NODISCARD ParserLine GetLineForPos(const TokenPos& pos) const override
    {
        return ParserLine();
    }
};
