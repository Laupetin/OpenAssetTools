#pragma once
#include "Loading/StreamProcessor.h"
#include "XChunks/IXChunkProcessor.h"

class ProcessorXChunks : public StreamProcessor
{
    class ProcessorXChunksImpl;
    ProcessorXChunksImpl* m_impl;

public:
    ProcessorXChunks(int numStreams, size_t xChunkSize);
    ~ProcessorXChunks() override;

    size_t Load(void* buffer, size_t length) override;

    void AddChunkProcessor(IXChunkProcessor* chunkProcessor) const;
};
