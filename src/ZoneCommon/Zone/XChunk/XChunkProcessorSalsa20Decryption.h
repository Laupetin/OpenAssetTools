#pragma once
#include "AbstractSalsa20Processor.h"
#include "IXChunkProcessor.h"

#include <string>

class XChunkProcessorSalsa20Decryption final : public IXChunkProcessor, public AbstractSalsa20Processor
{
public:
    XChunkProcessorSalsa20Decryption(unsigned streamCount, const std::string& zoneName, const uint8_t* salsa20Key, size_t keySize);

    size_t Process(unsigned streamNumber, const uint8_t* input, size_t inputLength, uint8_t* output, size_t outputBufferSize) override;
};
