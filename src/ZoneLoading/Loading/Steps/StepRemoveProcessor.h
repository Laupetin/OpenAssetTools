#pragma once

#include "Loading/ILoadingStep.h"

class StepRemoveProcessor final : public ILoadingStep
{
    StreamProcessor* m_stream_processor;
    bool m_removed;

public:
    explicit StepRemoveProcessor(StreamProcessor* streamProcessor);
    ~StepRemoveProcessor() override;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
