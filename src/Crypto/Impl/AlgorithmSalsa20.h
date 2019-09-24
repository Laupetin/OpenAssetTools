#pragma once
#include "IStreamCipher.h"

class AlgorithmSalsa20 final : public IStreamCipher
{
    class AlgorithmSalsa20Impl;
    AlgorithmSalsa20Impl* m_impl;

public:
    AlgorithmSalsa20(const uint8_t* keyBytes, size_t keySize);
    ~AlgorithmSalsa20() override;

    AlgorithmSalsa20(AlgorithmSalsa20& other);
    AlgorithmSalsa20(AlgorithmSalsa20&& other) noexcept;

    AlgorithmSalsa20& operator=(AlgorithmSalsa20 const& other);
    AlgorithmSalsa20& operator=(AlgorithmSalsa20&& other) noexcept;

    void SetIV(const uint8_t* iv, size_t ivSize) override;
    void Process(const void* plainText, void* cipherText, size_t amount) override;
};
