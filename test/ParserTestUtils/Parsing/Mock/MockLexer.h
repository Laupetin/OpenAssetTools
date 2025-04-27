#pragma once

#include "Parsing/ILexer.h"
#include "Utils/ClassUtils.h"

#include <concepts>
#include <iterator>
#include <vector>

template<std::derived_from<IParserValue> TokenType> class MockLexer final : public ILexer<TokenType>
{
public:
    MockLexer(std::initializer_list<Movable<TokenType>> tokens, TokenType eof)
        : m_tokens(std::make_move_iterator(tokens.begin()), std::make_move_iterator(tokens.end())),
          m_eof(std::move(eof)),
          m_pop_count(0u)
    {
    }

    MockLexer(std::vector<TokenType> tokens, TokenType eof)
        : m_tokens(std::move(tokens)),
          m_eof(std::move(eof)),
          m_pop_count(0u)
    {
    }

    ~MockLexer() override = default;
    MockLexer(const MockLexer& other) = delete;
    MockLexer(MockLexer&& other) noexcept = default;
    MockLexer& operator=(const MockLexer& other) = delete;
    MockLexer& operator=(MockLexer&& other) noexcept = default;

    const TokenType& GetToken(const size_t index) override
    {
        const auto absoluteIndex = m_pop_count + index;
        if (absoluteIndex < m_tokens.size())
            return m_tokens[absoluteIndex];

        return m_eof;
    }

    void PopTokens(const size_t amount) override
    {
        assert(amount >= 0);
        m_pop_count += amount;
    }

    bool IsEof() override
    {
        return !m_tokens.empty() || m_pop_count >= m_tokens.size();
    }

    const TokenPos& GetPos() override
    {
        return !m_tokens.empty() && m_pop_count < m_tokens.size() ? m_tokens[m_pop_count].GetPos() : m_eof.GetPos();
    }

    _NODISCARD size_t GetPopCount() const
    {
        return m_pop_count;
    }

    _NODISCARD ParserLine GetLineForPos(const TokenPos& pos) const override
    {
        return ParserLine();
    }

private:
    std::vector<TokenType> m_tokens;
    TokenType m_eof;
    size_t m_pop_count;
};
