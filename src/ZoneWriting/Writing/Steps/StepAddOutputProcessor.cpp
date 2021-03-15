#include "StepAddOutputProcessor.h"
#include <cassert>

StepAddOutputProcessor::StepAddOutputProcessor(std::unique_ptr<OutputStreamProcessor> streamProcessor)
    : m_stream_processor(std::move(streamProcessor))
{
}

void StepAddOutputProcessor::PerformStep(ZoneWriter* zoneLoader, IWritingStream* stream)
{
    assert(zoneLoader != nullptr);
    assert(m_stream_processor != nullptr);

    zoneLoader->AddStreamProcessor(std::move(m_stream_processor));
    m_stream_processor = nullptr;
}