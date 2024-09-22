#pragma once
#include <string>

namespace base64
{
    std::string EncodeBase64(const void* inputData, size_t inputLength);
    bool EncodeBase64(const void* inputData, size_t inputLength, void* outputBuffer, size_t outputBufferSize);
    size_t GetBase64EncodeOutputLength(size_t inputLength);

    size_t DecodeBase64(const void* base64Data, size_t inputLength, void* outputBuffer, size_t outputBufferSize);
    size_t GetBase64DecodeOutputLength(size_t inputLength);
} // namespace base64
