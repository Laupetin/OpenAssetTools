#include "MatcherResult.h"

namespace
{
    // The highest bit is the fabricated flag
    constexpr unsigned FABRICATED_FLAG_MASK = 1u << (sizeof(unsigned) * 8u - 1u);
    constexpr unsigned TOKEN_INDEX_MASK = ~FABRICATED_FLAG_MASK;
} // namespace

MatcherResultTokenIndex::MatcherResultTokenIndex(const unsigned index, const bool isFabricated)
{
    m_token_index = index & TOKEN_INDEX_MASK;
    if (isFabricated)
        m_token_index |= FABRICATED_FLAG_MASK;
}

bool MatcherResultTokenIndex::IsFabricated() const
{
    return m_token_index & FABRICATED_FLAG_MASK;
}

unsigned MatcherResultTokenIndex::GetTokenIndex() const
{
    return m_token_index & TOKEN_INDEX_MASK;
}

MatcherResultCapture::MatcherResultCapture(const int captureId, const unsigned tokenIndex)
    : MatcherResultCapture(captureId, tokenIndex, false)
{
}

MatcherResultCapture::MatcherResultCapture(const int captureId, const unsigned tokenIndex, const bool isFabricated)
    : m_capture_id(captureId),
      m_token_index(tokenIndex, isFabricated)
{
}

MatcherResultCapture::MatcherResultCapture(const int captureId, const MatcherResultTokenIndex index)
    : m_capture_id(captureId),
      m_token_index(index)
{
}

int MatcherResultCapture::GetCaptureId() const
{
    return m_capture_id;
}
