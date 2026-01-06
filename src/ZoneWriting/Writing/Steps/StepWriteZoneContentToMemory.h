#pragma once

#include "Writing/IContentWritingEntryPoint.h"
#include "Writing/IWritingStep.h"
#include "Zone/Stream/InMemoryZoneData.h"

#include <memory>

class StepWriteZoneContentToMemory final : public IWritingStep
{
public:
    StepWriteZoneContentToMemory(
        std::unique_ptr<IContentWritingEntryPoint> entryPoint, const Zone& zone, unsigned pointerBitCount, unsigned offsetBlockBitCount, block_t insertBlock);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
    [[nodiscard]] InMemoryZoneData* GetData() const;

private:
    std::unique_ptr<IContentWritingEntryPoint> m_content_loader;
    std::unique_ptr<InMemoryZoneData> m_zone_data;
    const Zone& m_zone;
    std::vector<XBlock*> m_blocks;

    unsigned m_pointer_bit_count;
    unsigned m_offset_block_bit_count;
    block_t m_insert_block;
};
