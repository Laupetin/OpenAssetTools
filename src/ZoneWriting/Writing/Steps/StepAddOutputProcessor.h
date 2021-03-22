#pragma once

#include <memory>

#include "Writing/OutputStreamProcessor.h"
#include "Writing/IWritingStep.h"

class StepAddOutputProcessor final : public IWritingStep
{
    std::unique_ptr<OutputStreamProcessor> m_stream_processor;

public:
    explicit StepAddOutputProcessor(std::unique_ptr<OutputStreamProcessor> streamProcessor);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
