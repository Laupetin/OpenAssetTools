#include "StepWriteZoneContentToMemory.h"

#include "Zone/Stream/Impl/InMemoryZoneOutputStream.h"

StepWriteZoneContentToMemory::StepWriteZoneContentToMemory(std::unique_ptr<IContentWritingEntryPoint> entryPoint,
                                                           const Zone& zone,
                                                           const int offsetBlockBitCount,
                                                           const block_t insertBlock)
    : m_content_loader(std::move(entryPoint)),
      m_zone_data(std::make_unique<InMemoryZoneData>()),
      m_zone(zone),
      m_offset_block_bit_count(offsetBlockBitCount),
      m_insert_block(insertBlock)
{
}

void StepWriteZoneContentToMemory::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    std::vector<XBlock*> blocks;
    blocks.reserve(zoneWriter->m_blocks.size());
    for (const auto& block : zoneWriter->m_blocks)
        blocks.emplace_back(block.get());

    const auto zoneOutputStream = std::make_unique<InMemoryZoneOutputStream>(m_zone_data.get(), std::move(blocks), m_offset_block_bit_count, m_insert_block);
    m_content_loader->WriteContent(*zoneOutputStream);
}

InMemoryZoneData* StepWriteZoneContentToMemory::GetData() const
{
    return m_zone_data.get();
}
