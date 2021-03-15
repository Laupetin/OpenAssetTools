#include "OutputProcessorXChunks.h"

class OutputProcessorXChunks::Impl
{
    
};

OutputProcessorXChunks::OutputProcessorXChunks(int numStreams, size_t xChunkSize)
{
}

OutputProcessorXChunks::OutputProcessorXChunks(int numStreams, size_t xChunkSize, size_t vanillaBufferSize)
{
}

OutputProcessorXChunks::~OutputProcessorXChunks()
{
    delete m_impl;
    m_impl = nullptr;
}

OutputProcessorXChunks::OutputProcessorXChunks(OutputProcessorXChunks&& other) noexcept
    : m_impl(other.m_impl)
{
    other.m_impl = nullptr;
}

OutputProcessorXChunks& OutputProcessorXChunks::operator=(OutputProcessorXChunks&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;
    return *this;
}

void OutputProcessorXChunks::AddChunkProcessor(std::unique_ptr<IXChunkOutputProcessor> chunkProcessor) const
{
}

void OutputProcessorXChunks::Write(const void* buffer, size_t length)
{
}

int64_t OutputProcessorXChunks::Pos()
{
    return 0;
}
