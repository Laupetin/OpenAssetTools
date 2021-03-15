#pragma once
#include <memory>

#include "Writing/OutputStreamProcessor.h"
#include "XChunks/IXChunkOutputProcessor.h"

class OutputProcessorXChunks final : public OutputStreamProcessor
{
    class Impl;
    Impl* m_impl;

public:
    OutputProcessorXChunks(int numStreams, size_t xChunkSize);
    OutputProcessorXChunks(int numStreams, size_t xChunkSize, size_t vanillaBufferSize);
    ~OutputProcessorXChunks() override;

    OutputProcessorXChunks(const OutputProcessorXChunks& other) = delete;
    OutputProcessorXChunks(OutputProcessorXChunks&& other) noexcept;
    OutputProcessorXChunks& operator=(const OutputProcessorXChunks& other) = delete;
    OutputProcessorXChunks& operator=(OutputProcessorXChunks&& other) noexcept;

    void AddChunkProcessor(std::unique_ptr<IXChunkOutputProcessor> chunkProcessor) const;

    void Write(const void* buffer, size_t length) override;
    int64_t Pos() override;
};
