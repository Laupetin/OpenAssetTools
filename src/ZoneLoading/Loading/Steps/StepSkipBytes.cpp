#include "StepSkipBytes.h"

StepSkipBytes::StepSkipBytes(const unsigned int skipCount)
{
    m_skip_count = skipCount;
}

void StepSkipBytes::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    uint8_t tempBuffer[128];
    unsigned int skippedBytes = 0;

    while(skippedBytes < m_skip_count)
    {
        unsigned int toSkip;

        if(m_skip_count - skippedBytes < sizeof(tempBuffer))
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