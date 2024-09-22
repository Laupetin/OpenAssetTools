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
        unsigned long outLength = outputBufferSize;
        const auto result = base64_encode(static_cast<const unsigned char*>(inputData), inputLength, static_cast<char*>(outputBuffer), &outLength);
        return result == CRYPT_OK;
    }

    size_t GetBase64EncodeOutputLength(const size_t inputLength)
    {
        return 4u * ((inputLength + 2u) / 3u);
    }

    size_t DecodeBase64(const void* base64Data, const size_t inputLength, void* outputBuffer, const size_t outputBufferSize)
    {
        unsigned long outLength = GetBase64DecodeOutputLength(inputLength);
        if (outLength > outputBufferSize)
            return 0u;

        const auto result = base64_decode(static_cast<const char*>(base64Data), inputLength, static_cast<unsigned char*>(outputBuffer), &outLength);
        assert(result == CRYPT_OK);

        return static_cast<size_t>(outLength);
    }

    size_t GetBase64DecodeOutputLength(const size_t inputLength)
    {
        return inputLength / 4u;
    }
} // namespace base64
