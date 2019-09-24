#include "StepLoadZoneContent.h"
#include "Zone/Stream/Impl/XBlockInputStream.h"
#include <cassert>

StepLoadZoneContent::StepLoadZoneContent(IContentLoadingEntryPoint* entryPoint, Zone* zone, const int offsetBlockBitCount, const block_t insertBlock)
{
    m_content_loader = entryPoint;
    m_zone = zone;
    m_offset_block_bit_count = offsetBlockBitCount;
    m_insert_block = insertBlock;
}

StepLoadZoneContent::~StepLoadZoneContent()
{
    delete m_content_loader;
    m_content_loader = nullptr;
}

void StepLoadZoneContent::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    auto* inputStream = new XBlockInputStream(zoneLoader->m_blocks, stream, m_offset_block_bit_count, m_insert_block);

    m_content_loader->Load(m_zone, inputStream);

    delete inputStream;
}