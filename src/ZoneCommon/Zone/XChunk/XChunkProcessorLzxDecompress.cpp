#include "XChunkProcessorLzxDecompress.h"

#include "Utils/Logging/Log.h"

XChunkProcessorLzxDecompress::XChunkProcessorLzxDecompress(const unsigned streamCount)
    : m_xmemdecompress_contexts(streamCount)
{
}

size_t XChunkProcessorLzxDecompress::Process(
    const unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    const auto& xMemDecompress = m_xmemdecompress_contexts[streamNumber];

    // lzx state is reset before each chunk
    xMemDecompress.Reset();

    const auto maybeDecompressedSize = xMemDecompress.Process(input, inputLength, output, outputBufferSize);
    if (!maybeDecompressedSize.has_value())
    {
        con::error("Failed to decompress xchunk with XMemDecompress");
        return 0;
    }

    return *maybeDecompressedSize;
}
