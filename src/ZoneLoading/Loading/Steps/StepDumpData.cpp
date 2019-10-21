#include "StepDumpData.h"

StepDumpData::StepDumpData(const unsigned int dumpCount)
{
    m_dump_count = dumpCount;
}

void StepDumpData::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    uint8_t tempBuffer[128];
    unsigned int dumpedBytes = 0;

    FileAPI::File tempFile = FileAPI::Open("dump.dat", FileAPI::Mode::MODE_WRITE);

    while (dumpedBytes < m_dump_count)
    {
        unsigned int toDump;

        if (m_dump_count - dumpedBytes < sizeof(tempBuffer))
        {
            toDump = m_dump_count - dumpedBytes;
        }
        else
        {
            toDump = sizeof(tempBuffer);
        }

        const size_t loadedSize = stream->Load(tempBuffer, toDump);
        dumpedBytes += loadedSize;

        if (loadedSize == 0)
            break;

        tempFile.Write(tempBuffer, 1, loadedSize);
    }

    tempFile.Close();
}