#pragma once

#include <functional>

#include "Parsing/IParserValue.h"
#include "Parsing/ILexer.h"
#include "Parsing/Matcher/MatcherResult.h"

template <typename TokenType>
class AbstractMatcher
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    static constexpr int NO_ID = -1;

private:
    int m_tag_id;
    int m_capture_id;
    bool m_no_consume;
    std::function<TokenType(std::vector<std::reference_wrapper<const TokenType>>&)> m_transform_func;

protected:
    AbstractMatcher()
        : m_tag_id(NO_ID),
          m_capture_id(NO_ID),
          m_no_consume(false)
    {
    }

    virtual MatcherResult<TokenType> CanMatch(ILexer<TokenType>* lexer, unsigned tokenOffset) = 0;

public:
    virtual ~AbstractMatcher() = default;
    AbstractMatcher(const AbstractMatcher& other) = default;
    AbstractMatcher(AbstractMatcher&& other) noexcept = default;
    AbstractMatcher& operator=(const AbstractMatcher& other) = default;
    AbstractMatcher& operator=(AbstractMatcher&& other) noexcept = default;

    void SetTag(const int tagId)
    {
        m_tag_id = tagId;
    }

    void SetCapture(const int captureId)
    {
        m_capture_id = captureId;
    }

    void SetConsume(const bool value)
    {
        m_no_consume = !value;
    }

    void SetTransform(std::function<TokenType(std::vector<std::reference_wrapper<const TokenType>>&)> transform)
    {
        m_transform_func = std::move(transform);
    }

    MatcherResult<TokenType> Match(ILexer<TokenType>* lexer, const unsigned tokenOffset)
    {
        MatcherResult<TokenType> result = CanMatch(lexer, tokenOffset);

        if (!result.m_matches)
            return result;

        if (m_tag_id != NO_ID)
            result.m_tags.push_back(m_tag_id);

        if (m_capture_id != NO_ID)
        {
            if (m_transform_func)
            {
                std::vector<std::reference_wrapper<const TokenType>> tokens;
                tokens.reserve(result.m_consumed_token_count);
                
                for (auto i = 0u; i < result.m_consumed_token_count; i++)
                    tokens.emplace_back(lexer->GetToken(tokenOffset + i));

                result.m_fabricated_tokens.push_back(m_transform_func(tokens));
                result.m_captures.emplace_back(m_capture_id, result.m_fabricated_tokens.size() - 1, true);
            }
            else
            {
                for (auto i = 0u; i < result.m_consumed_token_count; i++)
                {
                    result.m_captures.emplace_back(m_capture_id, tokenOffset + i);
                }
            }
        }

        return result;
    }
};
