#pragma once

#include <iterator>
#include <vector>

#include "Utils/ClassUtils.h"
#include "Parsing/IParserValue.h"

template <typename TokenType>
class MatcherResult
{
    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

public:
    class TokenIndex
    {
        static constexpr unsigned FABRICATED_FLAG_MASK = std::numeric_limits<unsigned>::max() ^ std::numeric_limits<int>::max();
        static constexpr unsigned TOKEN_INDEX_MASK = ~FABRICATED_FLAG_MASK;

        unsigned m_token_index;

    public:
        TokenIndex(const unsigned index, const bool isFabricated)
        {
            m_token_index = index & TOKEN_INDEX_MASK;
            if (isFabricated)
                m_token_index |= FABRICATED_FLAG_MASK;
        }

        _NODISCARD bool IsFabricated() const
        {
            return m_token_index & FABRICATED_FLAG_MASK;
        }

        _NODISCARD unsigned GetTokenIndex() const
        {
            return m_token_index & TOKEN_INDEX_MASK;
        }
    };

    class Capture
    {
    public:
        int m_capture_id;
        TokenIndex m_token_index;

        Capture(const int captureId, const unsigned tokenIndex)
            : Capture(captureId, tokenIndex, false)
        {
        }

        Capture(const int captureId, const unsigned tokenIndex, const bool isFabricated)
            : m_capture_id(captureId),
              m_token_index(tokenIndex, isFabricated)
        {
        }

        Capture(const int captureId, const TokenIndex index)
            : m_capture_id(captureId),
              m_token_index(index)
        {
        }

        _NODISCARD int GetCaptureId() const
        {
            return m_capture_id;
        }
    };

    bool m_matches;
    unsigned m_consumed_token_count;
    std::vector<int> m_tags;
    std::vector<Capture> m_captures;
    std::vector<TokenIndex> m_matched_tokens;
    std::vector<TokenType> m_fabricated_tokens;

private:
    MatcherResult(const bool matches, const unsigned consumedTokenCount)
        : m_matches(matches),
          m_consumed_token_count(consumedTokenCount)
    {
    }

public:
    static MatcherResult Match(unsigned consumedTokenCount)
    {
        return MatcherResult(true, consumedTokenCount);
    }

    static MatcherResult NoMatch()
    {
        return MatcherResult(false, 0);
    }

    void Absorb(MatcherResult<TokenType>&& other)
    {
        m_consumed_token_count += other.m_consumed_token_count;

        if (!other.m_tags.empty())
            std::copy(other.m_tags.begin(), other.m_tags.end(), std::back_inserter(m_tags));

        for (const auto& capture : other.m_captures)
        {
            if (capture.m_token_index.IsFabricated())
                m_captures.emplace_back(capture.GetCaptureId(), TokenIndex(m_fabricated_tokens.size() + capture.m_token_index.GetTokenIndex(), true));
            else
                m_captures.emplace_back(capture.GetCaptureId(), capture.m_token_index);
        }

        for (const auto& token : other.m_matched_tokens)
        {
            if (token.IsFabricated())
                m_matched_tokens.emplace_back(m_fabricated_tokens.size() + token.GetTokenIndex(), true);
            else
                m_matched_tokens.emplace_back(token.GetTokenIndex(), false);
        }

        for (auto& fabricated : other.m_fabricated_tokens)
        {
            m_fabricated_tokens.emplace_back(std::move(fabricated));
        }
    }
};
