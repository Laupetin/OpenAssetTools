#pragma once

#include "Loading/ILoadingStep.h"

class StepAllocXBlocks final : public ILoadingStep
{
public:
    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;
};
