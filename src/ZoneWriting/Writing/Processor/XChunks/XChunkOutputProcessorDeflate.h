#pragma once
#include "IXChunkOutputProcessor.h"

class XChunkOutputProcessorDeflate final : public IXChunkOutputProcessor
{
public:
    size_t Process(int streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;
};
