#include "XChunkProcessorLzxDecompress.h"

#include <cstring>
#include <format>
#include <iostream>
#include <lzx.h>

namespace
{
    uint8_t NextByte(const uint8_t* input, size_t& offset, size_t& remainingSize)
    {
        const auto value = input[offset];
        offset++;
        remainingSize--;
        return value;
    }

    uint16_t CombineHighLow(const uint8_t highByte, const uint8_t lowByte)
    {
        return static_cast<uint16_t>(static_cast<uint16_t>(static_cast<uint16_t>(highByte) << 8u) | static_cast<uint16_t>(lowByte));
    }

    void LogErrorHeaderSpace(size_t remainingInputSize)
    {
        std::cerr << std::format("XMemCompress: Not enough data for header: {}\n", remainingInputSize);
    }
} // namespace

XChunkProcessorLzxDecompress::XChunkProcessorLzxDecompress(const unsigned streamCount)
    : m_lzx_states(streamCount)
{
    // T6 uses 17 for window bits
    for (auto& lzxState : m_lzx_states)
        lzxState = lzx_init(17);
}

XChunkProcessorLzxDecompress::~XChunkProcessorLzxDecompress()
{
    for (auto* lzxState : m_lzx_states)
        lzx_teardown(static_cast<lzx_state*>(lzxState));
}

size_t XChunkProcessorLzxDecompress::Process(
    const unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    auto* state = static_cast<lzx_state*>(m_lzx_states[streamNumber]);

    // lzx state is reset before each chunk
    lzx_reset(state);

    size_t curInputOffset = 0uz;
    size_t curInputSize = inputLength;

    size_t curOutputOffset = 0uz;
    size_t curOutputSize = outputBufferSize;

    uint8_t lowByte;
    uint16_t dstSize, srcSize;

    while (curInputSize > 0)
    {
        uint8_t highByte = NextByte(input, curInputOffset, curInputSize);

        uint8_t suffixSize;
        if (highByte == 0xFF) // magic number: output is smaller than 0x8000
        {
            if (curInputSize < 4)
            {
                LogErrorHeaderSpace(curInputSize);
                return curOutputOffset;
            }

            highByte = NextByte(input, curInputOffset, curInputSize);
            lowByte = NextByte(input, curInputOffset, curInputSize);
            dstSize = CombineHighLow(highByte, lowByte);

            highByte = NextByte(input, curInputOffset, curInputSize);
            lowByte = NextByte(input, curInputOffset, curInputSize);
            srcSize = CombineHighLow(highByte, lowByte);

            // The game seems to skip a 5 byte suffix after these blocks, not sure why.
            suffixSize = 5u;
        }
        else
        {
            if (curInputSize < 1)
            {
                LogErrorHeaderSpace(curInputSize);
                return curOutputOffset;
            }

            dstSize = 0x8000u;
            lowByte = NextByte(input, curInputOffset, curInputSize);
            srcSize = CombineHighLow(highByte, lowByte);

            suffixSize = 0u;
        }

        if (srcSize == 0 || dstSize == 0)
        {
            // Other implementations do not handle this as a failure, game code suggests otherwise though
            std::cerr << std::format("XMemCompress: EOF: {} {}, {}\n", srcSize, dstSize, curInputSize);
            return curOutputOffset;
        }

        if (static_cast<size_t>(srcSize) + suffixSize > curInputSize)
        {
            std::cerr << std::format("XMemCompress: block size bigger than remaining data: {} > {}\n", srcSize, curInputSize);
            return curOutputOffset;
        }

        if (dstSize > curOutputSize)
        {
            std::cerr << std::format("XMemCompress: output size bigger than remaining data: {} > {}\n", dstSize, curOutputSize);
            return curOutputOffset;
        }

        auto ret = lzx_decompress(state, &input[curInputOffset], &output[curOutputOffset], srcSize, dstSize);
        curInputOffset += srcSize + suffixSize;
        curInputSize -= (srcSize + suffixSize);
        curOutputOffset += dstSize;
        curOutputSize -= srcSize;

        if (ret != DECR_OK)
        {
            std::cerr << std::format("XMemCompress: lzx decompression failed: {}\n", ret);
            return curOutputOffset;
        }
    }

    return curOutputOffset;
}
