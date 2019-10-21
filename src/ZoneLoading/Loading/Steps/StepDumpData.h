#pragma once

#include "Loading/ILoadingStep.h"

class StepDumpData final : public ILoadingStep
{
    unsigned int m_dump_count;

public:
    explicit StepDumpData(unsigned int dumpCount);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};