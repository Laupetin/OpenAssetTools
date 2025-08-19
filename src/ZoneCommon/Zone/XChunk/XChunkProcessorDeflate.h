#pragma once
#include "IXChunkProcessor.h"

class XChunkProcessorDeflate final : public IXChunkProcessor
{
public:
    size_t Process(unsigned streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;
};
