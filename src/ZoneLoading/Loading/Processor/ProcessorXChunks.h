#pragma once
#include <memory>

#include "Loading/StreamProcessor.h"
#include "XChunks/IXChunkProcessor.h"

class ProcessorXChunks : public StreamProcessor
{
    class ProcessorXChunksImpl;
    ProcessorXChunksImpl* m_impl;

public:
    ProcessorXChunks(int numStreams, size_t xChunkSize);
    ProcessorXChunks(int numStreams, size_t xChunkSize, size_t vanillaBufferSize);
    ~ProcessorXChunks() override;

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;

    void AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor) const;
};
