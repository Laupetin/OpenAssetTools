#pragma once

#include <cstddef>

#include "IHashFunction.h"
#include "IStreamCipher.h"
#include "IPublicKeyAlgorithm.h"

class Crypto
{
public:
    enum class RSAPaddingMode
    {
        RSA_PADDING_PKS1,
        RSA_PADDING_PSS,
    };

    static IHashFunction* CreateSHA1();
    static IHashFunction* CreateSHA256();

    static IStreamCipher* CreateSalsa20(const uint8_t* keyBytes, size_t keySize);

    static IPublicKeyAlgorithm* CreateRSA(IPublicKeyAlgorithm::HashingAlgorithm hashingAlgorithm, RSAPaddingMode paddingMode);
};
