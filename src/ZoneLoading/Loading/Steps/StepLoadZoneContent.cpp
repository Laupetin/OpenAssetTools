#include "StepLoadZoneContent.h"

#include "Zone/Stream/ZoneInputStream.h"

StepLoadZoneContent::StepLoadZoneContent(std::unique_ptr<IContentLoadingEntryPoint> entryPoint,
                                         Zone* zone,
                                         const int offsetBlockBitCount,
                                         const block_t insertBlock)
    : m_content_loader(std::move(entryPoint)),
      m_zone(zone),
      m_offset_block_bit_count(offsetBlockBitCount),
      m_insert_block(insertBlock)
{
}

void StepLoadZoneContent::PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream)
{
    const auto inputStream = ZoneInputStream::Create(zoneLoader.m_blocks, stream, m_offset_block_bit_count, m_insert_block);

    m_content_loader->Load(*inputStream);
}
