#include "StepSkipBytes.h"

StepSkipBytes::StepSkipBytes(const size_t skipCount)
    : m_skip_count(skipCount)
{
}

void StepSkipBytes::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    uint8_t tempBuffer[128];
    auto skippedBytes = 0uz;

    while (skippedBytes < m_skip_count)
    {
        size_t toSkip;

        if (m_skip_count - skippedBytes < sizeof(tempBuffer))
        {
            toSkip = m_skip_count - skippedBytes;
        }
        else
        {
            toSkip = sizeof(tempBuffer);
        }

        stream->Load(tempBuffer, toSkip);

        skippedBytes += toSkip;
    }
}
