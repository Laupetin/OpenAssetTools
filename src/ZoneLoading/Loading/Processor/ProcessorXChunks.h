#pragma once

#include "Game/IGame.h"
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

    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(unsigned numStreams, size_t xChunkSize, GameEndianness endianness);
    std::unique_ptr<IProcessorXChunks> CreateProcessorXChunks(unsigned numStreams, size_t xChunkSize, GameEndianness endianness, size_t vanillaBufferSize);
} // namespace processor
