#include "StepAddProcessor.h"

#include <cassert>

StepAddProcessor::StepAddProcessor(std::unique_ptr<StreamProcessor> streamProcessor)
    : m_stream_processor(std::move(streamProcessor))
{
}

void StepAddProcessor::PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream)
{
    assert(m_stream_processor != nullptr);

    zoneLoader.AddStreamProcessor(std::move(m_stream_processor));
    m_stream_processor = nullptr;
}
