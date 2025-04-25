#include "Base64.h"

#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>

namespace base64
{
    std::string EncodeBase64(const void* inputData, const size_t inputLength)
    {
        const auto base64BufferSize = GetBase64EncodeOutputLength(inputLength);

        std::string output(base64BufferSize, '\0');
        const auto outLength = base64BufferSize + 1u;

        const auto result = EncodeBase64(inputData, inputLength, output.data(), outLength);
        assert(result);

        return output;
    }

    bool EncodeBase64(const void* inputData, const size_t inputLength, void* outputBuffer, const size_t outputBufferSize)
    {
        unsigned long outLength = static_cast<unsigned long>(outputBufferSize);
        const auto result =
            base64_encode(static_cast<const unsigned char*>(inputData), static_cast<unsigned long>(inputLength), static_cast<char*>(outputBuffer), &outLength);
        return result == CRYPT_OK;
    }

    size_t GetBase64EncodeOutputLength(const size_t inputLength)
    {
        return 4u * ((inputLength + 2u) / 3u);
    }

    size_t DecodeBase64(const void* base64Data, const size_t inputLength, void* outputBuffer, const size_t outputBufferSize)
    {
        auto outLength = static_cast<unsigned long>(GetBase64DecodeOutputLength(base64Data, inputLength));
        assert(outLength <= outputBufferSize);
        if (outLength > outputBufferSize)
            return 0u;

        const auto result =
            base64_decode(static_cast<const char*>(base64Data), static_cast<unsigned long>(inputLength), static_cast<unsigned char*>(outputBuffer), &outLength);
        assert(result == CRYPT_OK);

        return static_cast<size_t>(outLength);
    }

    size_t GetBase64DecodeOutputLength(const void* base64Data, const size_t inputLength)
    {
        assert(base64Data);
        assert(inputLength);

        if (!base64Data || inputLength == 0u)
            return 0u;

        auto padding = 0u;
        if (inputLength >= 1 && static_cast<const char*>(base64Data)[inputLength - 1] == '=')
        {
            if (inputLength >= 2 && static_cast<const char*>(base64Data)[inputLength - 2] == '=')
                padding = 2u;
            else
                padding = 1u;
        }

        return ((inputLength / 4u) * 3u) - padding;
    }

    size_t GetBase64DecodeOutputLength(const size_t inputLength)
    {
        return (inputLength / 4u) * 3u;
    }
} // namespace base64
