#pragma once

#include <cstdint>
#include <cstdlib>

namespace cryptography
{
    class IStreamCipher
    {
    public:
        IStreamCipher() = default;
        virtual ~IStreamCipher() = default;
        IStreamCipher(const IStreamCipher& other) = default;
        IStreamCipher(IStreamCipher&& other) noexcept = default;
        IStreamCipher& operator=(const IStreamCipher& other) = default;
        IStreamCipher& operator=(IStreamCipher&& other) noexcept = default;

        virtual void SetIv(const uint8_t* iv, size_t ivSize) = 0;
        virtual void Process(const void* plainText, void* cipherText, size_t amount) = 0;
    };
} // namespace cryptography
