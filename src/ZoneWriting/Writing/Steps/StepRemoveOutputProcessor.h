#pragma once

#include "Writing/OutputStreamProcessor.h"
#include "Writing/IWritingStep.h"

class StepRemoveOutputProcessor final : public IWritingStep
{
    OutputStreamProcessor* m_stream_processor;

public:
    explicit StepRemoveOutputProcessor(OutputStreamProcessor* streamProcessor);

    void PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream) override;
};
