#include "StreamProcessor.h"

StreamProcessor::StreamProcessor()
{
    m_base_stream = nullptr;
}

void StreamProcessor::SetBaseStream(ILoadingStream* baseStream)
{
    m_base_stream = baseStream;
}

void StreamProcessor::Skip(size_t length)
{
    m_base_stream->Skip(length);
}

int64_t StreamProcessor::Pos()
{
    return m_base_stream->Pos();
}
