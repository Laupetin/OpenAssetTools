#pragma once
#include "StepWriteZoneContentToMemory.h"
#include "Writing/IWritingStep.h"

class StepWriteZoneSizes final : public IWritingStep
{
    StepWriteZoneContentToMemory* m_memory;

public:
    explicit StepWriteZoneSizes(StepWriteZoneContentToMemory* memory);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
