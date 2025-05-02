#pragma once

#include "Loading/ILoadingStep.h"

class StepRemoveProcessor final : public ILoadingStep
{
public:
    explicit StepRemoveProcessor(StreamProcessor* streamProcessor);

    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;

private:
    StreamProcessor* m_stream_processor;
};
