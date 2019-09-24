#pragma once

#include "Loading/ILoadingStep.h"

class StepAddProcessor final : public ILoadingStep
{
    StreamProcessor* m_stream_processor;

public:
    explicit StepAddProcessor(StreamProcessor* streamProcessor);
    ~StepAddProcessor() override;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
