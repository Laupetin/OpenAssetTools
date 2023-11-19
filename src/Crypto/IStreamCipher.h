#pragma once

#include <cstddef>
#include <cstdint>

class IStreamCipher
{
public:
    virtual ~IStreamCipher() = default;

    virtual void SetIV(const uint8_t* iv, size_t ivSize) = 0;

    virtual void Process(const void* plainText, void* cipherText, size_t amount) = 0;
};
