#include "StepWriteZero.h"

#include <algorithm>

StepWriteZero::StepWriteZero(const size_t count)
    : m_count(count)
{
}

void StepWriteZero::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    constexpr uint64_t num = 0;
    size_t toWrite;

    for (auto i = 0uz; i < m_count; i += toWrite)
    {
        toWrite = std::min(sizeof(uint64_t), m_count - i);
        stream->Write(&num, toWrite);
    }
}
