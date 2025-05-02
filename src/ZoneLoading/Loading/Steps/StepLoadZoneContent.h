#pragma once

#include "Loading/IContentLoadingEntryPoint.h"
#include "Loading/ILoadingStep.h"

#include <memory>

class StepLoadZoneContent final : public ILoadingStep
{
public:
    StepLoadZoneContent(std::unique_ptr<IContentLoadingEntryPoint> entryPoint, Zone* zone, int offsetBlockBitCount, block_t insertBlock);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;

private:
    std::unique_ptr<IContentLoadingEntryPoint> m_content_loader;
    Zone* m_zone;
    int m_offset_block_bit_count;
    block_t m_insert_block;
};
