#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>

#include "Writing/OutputStreamProcessor.h"
#include "Zone/XChunk/IXChunkProcessor.h"

class OutputProcessorXChunks final : public OutputStreamProcessor
{
    std::vector<std::unique_ptr<IXChunkProcessor>> m_chunk_processors;

    int m_stream_count;
    size_t m_chunk_size;
    size_t m_vanilla_buffer_size;

    bool m_initialized;
    int m_current_stream;
    size_t m_vanilla_buffer_offset;

    std::vector<std::unique_ptr<uint8_t[]>> m_buffers;
    uint8_t* m_input_buffer;
    uint8_t* m_output_buffer;
    size_t m_input_size;

    void Init();
    void WriteChunk();

public:
    OutputProcessorXChunks(int numStreams, size_t xChunkSize);
    OutputProcessorXChunks(int numStreams, size_t xChunkSize, size_t vanillaBufferSize);
    ~OutputProcessorXChunks() override = default;

    OutputProcessorXChunks(const OutputProcessorXChunks& other) = delete;
    OutputProcessorXChunks(OutputProcessorXChunks&& other) noexcept = default;
    OutputProcessorXChunks& operator=(const OutputProcessorXChunks& other) = delete;
    OutputProcessorXChunks& operator=(OutputProcessorXChunks&& other) noexcept = default;

    void AddChunkProcessor(std::unique_ptr<IXChunkProcessor> chunkProcessor);

    void Write(const void* buffer, size_t length) override;
    void Flush() override;
    int64_t Pos() override;
};
