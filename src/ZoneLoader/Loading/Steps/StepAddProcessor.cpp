#include "StepAddProcessor.h"
#include <cassert>

StepAddProcessor::StepAddProcessor(StreamProcessor* streamProcessor)
{
    m_stream_processor = streamProcessor;
}

StepAddProcessor::~StepAddProcessor()
{
    delete m_stream_processor;
    m_stream_processor = nullptr;
}

void StepAddProcessor::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(zoneLoader != nullptr);
    assert(m_stream_processor != nullptr);

    zoneLoader->AddStreamProcessor(m_stream_processor);
    m_stream_processor = nullptr;
}