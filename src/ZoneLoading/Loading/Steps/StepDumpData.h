#pragma once

#include "Loading/ILoadingStep.h"

class StepDumpData final : public ILoadingStep
{
public:
    explicit StepDumpData(size_t dumpCount);

    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;

private:
    size_t m_dump_count;
};
