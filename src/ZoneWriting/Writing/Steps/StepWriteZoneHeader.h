#pragma once
#include "Writing/IWritingStep.h"

class StepWriteZoneHeader final : public IWritingStep
{
    ZoneHeader m_header;

public:
    explicit StepWriteZoneHeader(ZoneHeader header);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
