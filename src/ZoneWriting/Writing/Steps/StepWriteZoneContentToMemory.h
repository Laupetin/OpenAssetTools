#pragma once

#include <memory>

#include "Utils/ClassUtils.h"
#include "Writing/IContentWritingEntryPoint.h"
#include "Writing/InMemoryZoneData.h"
#include "Writing/IWritingStep.h"

class StepWriteZoneContentToMemory final : public IWritingStep
{
    std::unique_ptr<IContentWritingEntryPoint> m_content_loader;
    std::unique_ptr<InMemoryZoneData> m_zone_data;
    Zone* m_zone;
    int m_offset_block_bit_count;
    block_t m_insert_block;

public:
    StepWriteZoneContentToMemory(std::unique_ptr<IContentWritingEntryPoint> entryPoint, Zone* zone, int offsetBlockBitCount, block_t insertBlock);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
    _NODISCARD InMemoryZoneData* GetData() const;
};
