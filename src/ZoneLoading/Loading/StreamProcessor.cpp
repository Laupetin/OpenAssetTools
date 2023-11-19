#include "StreamProcessor.h"

StreamProcessor::StreamProcessor()
{
    m_base_stream = nullptr;
}

void StreamProcessor::SetBaseStream(ILoadingStream* baseStream)
{
    m_base_stream = baseStream;
}
