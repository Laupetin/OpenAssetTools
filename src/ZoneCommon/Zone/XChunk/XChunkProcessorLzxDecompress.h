#pragma once

#include "IXChunkProcessor.h"

#include <vector>

class XChunkProcessorLzxDecompress final : public IXChunkProcessor
{
public:
    explicit XChunkProcessorLzxDecompress(unsigned streamCount);
    ~XChunkProcessorLzxDecompress();
    size_t Process(unsigned streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;

private:
    std::vector<void*> m_lzx_states;
};
