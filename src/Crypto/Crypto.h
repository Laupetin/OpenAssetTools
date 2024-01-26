#pragma once

#include "IHashFunction.h"
#include "IPublicKeyAlgorithm.h"
#include "IStreamCipher.h"

#include <cstddef>
#include <memory>

class Crypto
{
public:
    enum class RSAPaddingMode
    {
        RSA_PADDING_PKS1,
        RSA_PADDING_PSS,
    };

    static std::unique_ptr<IHashFunction> CreateMD5();

    static std::unique_ptr<IHashFunction> CreateSHA1();
    static std::unique_ptr<IHashFunction> CreateSHA256();

    static std::unique_ptr<IStreamCipher> CreateSalsa20(const uint8_t* keyBytes, size_t keySize);

    static std::unique_ptr<IPublicKeyAlgorithm> CreateRSA(IPublicKeyAlgorithm::HashingAlgorithm hashingAlgorithm, RSAPaddingMode paddingMode);
};
