#include "StepWriteZoneSizes.h"

StepWriteZoneSizes::StepWriteZoneSizes(StepWriteZoneContentToMemory* memory)
    : m_memory(memory)
{
}

void StepWriteZoneSizes::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    auto totalSize = static_cast<size_t>(m_memory->GetData()->m_total_size);
    size_t externalSize = 0;

    stream->Write(&totalSize, sizeof(totalSize));
    stream->Write(&externalSize, sizeof(externalSize));
}
