#pragma once

#include "Parsing/IParserValue.h"

#include <algorithm>
#include <concepts>
#include <iterator>
#include <vector>

class MatcherResultTokenIndex
{
public:
    MatcherResultTokenIndex(size_t index, bool isFabricated);
    [[nodiscard]] bool IsFabricated() const;
    [[nodiscard]] size_t GetTokenIndex() const;

private:
    size_t m_token_index;
};

class MatcherResultCapture
{
public:
    MatcherResultCapture(int captureId, unsigned tokenIndex);
    MatcherResultCapture(int captureId, unsigned tokenIndex, bool isFabricated);
    MatcherResultCapture(int captureId, MatcherResultTokenIndex index);

    [[nodiscard]] int GetCaptureId() const;

    int m_capture_id;
    MatcherResultTokenIndex m_token_index;
};

template<std::derived_from<IParserValue> TokenType> class MatcherResult
{
public:
    static MatcherResult Match(const unsigned consumedTokenCount)
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
            std::ranges::copy(other.m_tags, std::back_inserter(m_tags));

        for (const auto& capture : other.m_captures)
        {
            if (capture.m_token_index.IsFabricated())
                m_captures.emplace_back(capture.GetCaptureId(),
                                        MatcherResultTokenIndex(m_fabricated_tokens.size() + capture.m_token_index.GetTokenIndex(), true));
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

    bool m_matches;
    unsigned m_consumed_token_count;
    std::vector<int> m_tags;
    std::vector<MatcherResultCapture> m_captures;
    std::vector<MatcherResultTokenIndex> m_matched_tokens;
    std::vector<TokenType> m_fabricated_tokens;

private:
    MatcherResult(const bool matches, const unsigned consumedTokenCount)
        : m_matches(matches),
          m_consumed_token_count(consumedTokenCount)
    {
    }
};
