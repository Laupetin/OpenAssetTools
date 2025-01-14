#pragma once
#include "Writing/IWritingStep.h"
#include "Zone/Zone.h"

class StepWriteXBlockSizes final : public IWritingStep
{
public:
    explicit StepWriteXBlockSizes(const Zone& zone);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;

private:
    const Zone& m_zone;
};
