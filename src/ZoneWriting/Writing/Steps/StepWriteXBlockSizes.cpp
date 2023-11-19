#include "StepWriteXBlockSizes.h"

StepWriteXBlockSizes::StepWriteXBlockSizes(Zone* zone)
    : m_zone(zone)
{
}

void StepWriteXBlockSizes::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    for (const auto& block : zoneWriter->m_blocks)
    {
        auto blockSize = static_cast<xblock_size_t>(block->m_buffer_size);
        stream->Write(&blockSize, sizeof(blockSize));
    }
}
