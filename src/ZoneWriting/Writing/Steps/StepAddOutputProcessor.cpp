#include "StepAddOutputProcessor.h"
#include <cassert>

StepAddOutputProcessor::StepAddOutputProcessor(std::unique_ptr<OutputStreamProcessor> streamProcessor)
    : m_stream_processor(std::move(streamProcessor))
{
}

void StepAddOutputProcessor::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    assert(zoneWriter != nullptr);
    assert(m_stream_processor != nullptr);

    stream->Flush();
    zoneWriter->AddStreamProcessor(std::move(m_stream_processor));
    m_stream_processor = nullptr;
}