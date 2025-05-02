#pragma once

#include "Loading/ILoadingStep.h"

class StepSkipBytes final : public ILoadingStep
{
public:
    explicit StepSkipBytes(size_t skipCount);

    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;

private:
    size_t m_skip_count;
};
