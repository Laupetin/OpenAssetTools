#pragma once
#include "Writing/IWritingStep.h"


class StepWriteZoneRSA final : public IWritingStep
{
    std::string m_zoneName;

public:
    explicit StepWriteZoneRSA(std::string zoneName);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
