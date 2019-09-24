#pragma once
#include <cstdint>

class IXChunkProcessor
{
public:
    virtual ~IXChunkProcessor() = default;
    virtual size_t Process(int streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) = 0;
};
