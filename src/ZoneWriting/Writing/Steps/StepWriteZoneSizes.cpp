#include "StepWriteZoneSizes.h"

#include <cstdint>

StepWriteZoneSizes::StepWriteZoneSizes(StepWriteZoneContentToMemory* memory)
    : m_memory(memory)
{
}

void StepWriteZoneSizes::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    const auto totalSize = static_cast<uint32_t>(m_memory->GetData()->m_total_size);
    constexpr uint32_t externalSize = 0;

    stream->Write(&totalSize, sizeof(totalSize));
    stream->Write(&externalSize, sizeof(externalSize));
}
