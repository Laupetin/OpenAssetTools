#pragma once
#include "Writing/IWritingStep.h"

class StepWriteZero final : public IWritingStep
{
    size_t m_count;

public:
    explicit StepWriteZero(size_t count);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
