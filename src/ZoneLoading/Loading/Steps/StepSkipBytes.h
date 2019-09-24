#pragma once

#include "Loading/ILoadingStep.h"

class StepSkipBytes final : public ILoadingStep
{
    unsigned int m_skip_count;

public:
    explicit StepSkipBytes(unsigned int skipCount);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};