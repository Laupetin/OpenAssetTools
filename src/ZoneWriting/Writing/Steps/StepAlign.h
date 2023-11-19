#pragma once

#include "Writing/IWritingStep.h"

#include <cstddef>
#include <cstdint>

class StepAlign final : public IWritingStep
{
    size_t m_align_to;
    uint8_t m_align_value;

public:
    StepAlign(size_t alignTo, uint8_t alignValue);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
