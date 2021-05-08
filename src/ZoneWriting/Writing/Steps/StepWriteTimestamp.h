#pragma once
#include "Writing/IWritingStep.h"

class StepWriteTimestamp final : public IWritingStep
{
public:
    StepWriteTimestamp();

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
