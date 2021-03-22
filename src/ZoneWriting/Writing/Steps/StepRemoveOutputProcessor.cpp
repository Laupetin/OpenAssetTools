#include "StepRemoveOutputProcessor.h"

StepRemoveOutputProcessor::StepRemoveOutputProcessor(OutputStreamProcessor* streamProcessor)
    : m_stream_processor(streamProcessor)
{
}

void StepRemoveOutputProcessor::PerformStep(ZoneWriter* zoneWriter, IWritingStream* stream)
{
    stream->Flush();
    zoneWriter->RemoveStreamProcessor(m_stream_processor);
}
