#include "StepRemoveProcessor.h"
#include <cassert>

StepRemoveProcessor::StepRemoveProcessor(StreamProcessor* streamProcessor)
    : m_stream_processor(streamProcessor),
      m_removed(false)
{
}

StepRemoveProcessor::~StepRemoveProcessor()
{
    if(m_removed)
    {
        delete m_stream_processor;
        m_stream_processor = nullptr;
    }
}

void StepRemoveProcessor::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(zoneLoader != nullptr);
    assert(m_stream_processor != nullptr);

    zoneLoader->RemoveStreamProcessor(m_stream_processor);
    m_removed = true;
}
