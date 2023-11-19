#include "OutputStreamProcessor.h"

OutputStreamProcessor::OutputStreamProcessor()
{
    m_base_stream = nullptr;
}

void OutputStreamProcessor::SetBaseStream(IWritingStream* baseStream)
{
    m_base_stream = baseStream;
}
