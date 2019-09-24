#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/IContentLoadingEntryPoint.h"

class StepLoadZoneContent final : public ILoadingStep
{
    IContentLoadingEntryPoint* m_content_loader;
    Zone* m_zone;
    int m_offset_block_bit_count;
    block_t m_insert_block;

public:
    StepLoadZoneContent(IContentLoadingEntryPoint* entryPoint, Zone* zone, int offsetBlockBitCount, block_t insertBlock);
    ~StepLoadZoneContent();

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
