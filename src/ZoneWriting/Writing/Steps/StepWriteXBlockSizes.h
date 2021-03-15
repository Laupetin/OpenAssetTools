#pragma once
#include "Writing/IWritingStep.h"
#include "Zone/Zone.h"

class StepWriteXBlockSizes final : public IWritingStep
{
    Zone* m_zone;

public:
    explicit StepWriteXBlockSizes(Zone* zone);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
