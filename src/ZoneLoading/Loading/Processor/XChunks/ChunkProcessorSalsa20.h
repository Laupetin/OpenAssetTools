#pragma once
#include "IXChunkProcessor.h"
#include "Loading/ISignatureDataProvider.h"
#include <string>

class ChunkProcessorSalsa20 : public IXChunkProcessor, public ISignatureDataProvider
{
    class ChunkProcessorSalsa20Impl;
    ChunkProcessorSalsa20Impl* m_impl;

public:
    ChunkProcessorSalsa20(int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize);
    ~ChunkProcessorSalsa20() override;

    size_t Process(int streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;
    void GetSignatureData(const uint8_t** pSignatureData, size_t* pSize) override;
};
