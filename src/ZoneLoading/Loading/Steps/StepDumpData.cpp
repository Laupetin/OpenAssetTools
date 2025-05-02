#include "StepDumpData.h"

#include <fstream>

StepDumpData::StepDumpData(const size_t dumpCount)
    : m_dump_count(dumpCount)
{
}

void StepDumpData::PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream)
{
    uint8_t tempBuffer[128];
    auto dumpedBytes = 0uz;

    std::ofstream tempFile("dump.dat", std::fstream::out | std::fstream::binary);

    while (dumpedBytes < m_dump_count)
    {
        size_t toDump;

        if (m_dump_count - dumpedBytes < sizeof(tempBuffer))
        {
            toDump = m_dump_count - dumpedBytes;
        }
        else
        {
            toDump = sizeof(tempBuffer);
        }

        const auto loadedSize = stream.Load(tempBuffer, toDump);
        dumpedBytes += loadedSize;

        if (loadedSize == 0)
            break;

        tempFile.write(reinterpret_cast<char*>(tempBuffer), static_cast<std::streamsize>(loadedSize));
    }

    tempFile.close();
}
