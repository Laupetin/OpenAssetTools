#include "MatcherResult.h"

namespace
{
    // The highest bit is the fabricated flag
    constexpr size_t FABRICATED_FLAG_MASK = 1uz << (sizeof(size_t) * 8uz - 1uz);
    constexpr size_t TOKEN_INDEX_MASK = ~FABRICATED_FLAG_MASK;
} // namespace

MatcherResultTokenIndex::MatcherResultTokenIndex(const size_t index, const bool isFabricated)
{
    m_token_index = index & TOKEN_INDEX_MASK;
    if (isFabricated)
        m_token_index |= FABRICATED_FLAG_MASK;
}

bool MatcherResultTokenIndex::IsFabricated() const
{
    return m_token_index & FABRICATED_FLAG_MASK;
}

size_t MatcherResultTokenIndex::GetTokenIndex() const
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
