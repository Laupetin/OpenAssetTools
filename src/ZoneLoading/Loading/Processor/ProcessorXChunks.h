#pragma once
#include "Loading/StreamProcessor.h"
#include "Zone/XChunk/IXChunkProcessor.h"

#include <memory>

namespace processor
{
    class IProcessorXChunks : public StreamProcessor
    {
    public:
        virtual void AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor) = 0;
    };

    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(int numStreams, size_t xChunkSize);
    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(int numStreams, size_t xChunkSize, size_t vanillaBufferSize);
} // namespace processor
