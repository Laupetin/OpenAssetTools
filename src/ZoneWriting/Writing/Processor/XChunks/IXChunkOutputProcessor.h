#pragma once

#include <cstdint>
#include <cstddef>

class IXChunkOutputProcessor
{
public:
    IXChunkOutputProcessor() = default;
    virtual ~IXChunkOutputProcessor() = default;
    IXChunkOutputProcessor(const IXChunkOutputProcessor& other) = default;
    IXChunkOutputProcessor(IXChunkOutputProcessor&& other) noexcept = default;
    IXChunkOutputProcessor& operator=(const IXChunkOutputProcessor& other) = default;
    IXChunkOutputProcessor& operator=(IXChunkOutputProcessor&& other) noexcept = default;

    virtual size_t Process(int streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) = 0;
};
