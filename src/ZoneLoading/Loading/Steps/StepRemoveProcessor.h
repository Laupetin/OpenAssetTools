#pragma once

#include "Loading/ILoadingStep.h"

class StepRemoveProcessor final : public ILoadingStep
{
    StreamProcessor* m_stream_processor;

public:
    explicit StepRemoveProcessor(StreamProcessor* streamProcessor);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
