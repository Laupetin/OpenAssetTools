#include "StepWriteZoneContentToMemory.h"

#include "Zone/Stream/ZoneOutputStream.h"

StepWriteZoneContentToMemory::StepWriteZoneContentToMemory(std::unique_ptr<IContentWritingEntryPoint> entryPoint,
                                                           const Zone& zone,
                                                           const unsigned pointerBitCount,
                                                           const unsigned zonePointerOffsetBitCount,
                                                           const unsigned zonePointerBlockBitCount,
                                                           const block_t insertBlock)
    : m_content_loader(std::move(entryPoint)),
      m_zone_data(std::make_unique<InMemoryZoneData>()),
      m_zone(zone),
      m_pointer_bit_count(pointerBitCount),
      m_zone_pointer_offset_bit_count(zonePointerOffsetBitCount),
      m_zone_pointer_block_bit_count(zonePointerBlockBitCount),
      m_insert_block(insertBlock)
{
}

void StepWriteZoneContentToMemory::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    m_blocks.reserve(zoneWriter->m_blocks.size());
    for (const auto& block : zoneWriter->m_blocks)
        m_blocks.emplace_back(block.get());

    const auto zoneOutputStream =
        ZoneOutputStream::Create(m_pointer_bit_count, m_zone_pointer_offset_bit_count, m_zone_pointer_block_bit_count, m_blocks, m_insert_block, *m_zone_data);
    m_content_loader->WriteContent(*zoneOutputStream);
}

InMemoryZoneData* StepWriteZoneContentToMemory::GetData() const
{
    return m_zone_data.get();
}
