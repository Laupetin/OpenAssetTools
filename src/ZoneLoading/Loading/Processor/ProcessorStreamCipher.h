#pragma once
#include "Crypto.h"
#include "Loading/StreamProcessor.h"

class ProcessorStreamCipher final : public StreamProcessor
{
    IStreamCipher* m_cipher;

public:
    explicit ProcessorStreamCipher(IStreamCipher* cipher);
    ~ProcessorStreamCipher() override;

    size_t Load(void* buffer, size_t length) override;
};
