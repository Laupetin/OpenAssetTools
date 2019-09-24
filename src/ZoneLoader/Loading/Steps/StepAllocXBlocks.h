#pragma once

#include "Loading/ILoadingStep.h"

class StepAllocXBlocks final : public ILoadingStep
{
    static const uint64_t MAX_XBLOCK_SIZE;

public:
    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
