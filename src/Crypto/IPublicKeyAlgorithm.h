#pragma once

#include <cstdint>

class IPublicKeyAlgorithm
{
public:
    enum HashingAlgorithm
    {
        RSA_HASH_SHA256,
        RSA_HASH_SHA512
    };

    virtual ~IPublicKeyAlgorithm() = default;

    virtual bool SetKey(const uint8_t* keyData, size_t keySize) = 0;

    // If needed add a signing method

    virtual bool Verify(const uint8_t* signedData, size_t signedDataSize, const uint8_t* signature, size_t signatureSize) = 0;
};