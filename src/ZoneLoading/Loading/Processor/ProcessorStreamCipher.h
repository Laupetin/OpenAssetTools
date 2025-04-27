#pragma once

#include "Cryptography.h"
#include "Loading/StreamProcessor.h"

#include <memory>

class ProcessorStreamCipher final : public StreamProcessor
{
public:
    explicit ProcessorStreamCipher(std::unique_ptr<cryptography::IStreamCipher> cipher);

    size_t Load(void* buffer, size_t length) override;

private:
    std::unique_ptr<cryptography::IStreamCipher> m_cipher;
};
