#pragma once

#include <cstddef>
#include <cstdint>

class IXChunkProcessor
{
public:
    IXChunkProcessor() = default;
    virtual ~IXChunkProcessor() = default;
    IXChunkProcessor(const IXChunkProcessor& other) = default;
    IXChunkProcessor(IXChunkProcessor&& other) noexcept = default;
    IXChunkProcessor& operator=(const IXChunkProcessor& other) = default;
    IXChunkProcessor& operator=(IXChunkProcessor&& other) noexcept = default;

    virtual size_t Process(unsigned streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) = 0;
};
