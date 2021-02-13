#pragma once

#include <unordered_map>

#include "Utils/ClassUtils.h"
#include "Parsing/Matcher/AbstractMatcher.h"
#include "Parsing/Matcher/MatcherResult.h"
#include "Parsing/ParsingException.h"

template <typename TokenType>
class SequenceResult
{
    class Capture
    {
    public:
        unsigned m_offset;
        std::vector<std::reference_wrapper<const TokenType>> m_tokens;

        Capture()
            : m_offset(0)
        {
        }
    };

    // TokenType must inherit IParserValue
    static_assert(std::is_base_of<IParserValue, TokenType>::value);

    std::vector<int> m_tags;
    std::unordered_map<int, Capture> m_captures;

    unsigned m_tag_offset;

public:
    SequenceResult(ILexer<TokenType>* lexer, const MatcherResult<TokenType>& result)
        : m_tags(result.m_tags),
          m_tag_offset(0)
    {
        for (const typename MatcherResult<TokenType>::Capture& capture : result.m_captures)
        {
            if (capture.IsFabricated())
                m_captures[capture.GetCaptureId()].m_tokens.push_back(result.m_fabricated_tokens[capture.GetTokenIndex()]);
            else
                m_captures[capture.GetCaptureId()].m_tokens.push_back(lexer->GetToken(capture.GetTokenIndex()));
        }
    }

    _NODISCARD int PeekTag() const
    {
        if (m_tag_offset < m_tags.size())
            return m_tags[m_tag_offset];

        return AbstractMatcher<TokenType>::NO_ID;
    }

    int NextTag()
    {
        if (m_tag_offset < m_tags.size())
            return m_tags[m_tag_offset++];

        return AbstractMatcher<TokenType>::NO_ID;
    }

    _NODISCARD bool HasNextCapture(int captureId)
    {
        auto foundEntry = m_captures.find(captureId);

        if (foundEntry == m_captures.end())
            return false;

        return foundEntry->second.m_offset < foundEntry->second.m_tokens.size();
    }

    const TokenType& NextCapture(int captureId)
    {
        auto foundEntry = m_captures.find(captureId);

        if (foundEntry == m_captures.end())
            throw ParsingException(TokenPos(), "Tried to access next capture even though no captures exists!");

        if(foundEntry->second.m_offset >= foundEntry->second.m_tokens.size())
            throw ParsingException(TokenPos(), "Tried to access next capture even though none exists!");

        return foundEntry->second.m_tokens[foundEntry->second.m_offset++];
    }
};
