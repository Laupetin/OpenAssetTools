#include "StepRemoveProcessor.h"

#include <cassert>

StepRemoveProcessor::StepRemoveProcessor(StreamProcessor* streamProcessor)
    : m_stream_processor(streamProcessor)
{
}

void StepRemoveProcessor::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(zoneLoader != nullptr);
    assert(m_stream_processor != nullptr);

    zoneLoader->RemoveStreamProcessor(m_stream_processor);
}
