#pragma once
#include "StepWriteZoneContentToMemory.h"
#include "Writing/IWritingStep.h"

class StepWriteZoneContentToFile final : public IWritingStep
{
    StepWriteZoneContentToMemory* m_memory;

public:
    explicit StepWriteZoneContentToFile(StepWriteZoneContentToMemory* zoneMemory);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
