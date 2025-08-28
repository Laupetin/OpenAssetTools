#include "XChunkProcessorLzxDecompress.h"

#include <cstring>
#include <format>
#include <iostream>
#include <lzx.h>

namespace
{
    // struct LzxInput
    // {
    //     const uint8_t* input;
    //     const size_t inputLength;
    // };

    uint8_t NextByte(const uint8_t* input, size_t& offset, size_t& remainingSize)
    {
        const auto value = input[offset];
        offset++;
        remainingSize--;
        return value;
    }

    uint16_t CombineHiLo(uint8_t hi, uint8_t lo)
    {
        return (static_cast<uint16_t>(hi) << 8u) | static_cast<uint16_t>(lo);
    }

    void LogErrorHeaderSpace(size_t remainingInputSize)
    {
        std::cerr << std::format("XMemCompress: Not enough data for header: {}\n", remainingInputSize);
    }
} // namespace

XChunkProcessorLzxDecompress::XChunkProcessorLzxDecompress(unsigned streamCount)
    : m_lzx_states(streamCount)
{
    for (auto& lzxState : m_lzx_states)
        lzxState = lzx_init(16);
}

XChunkProcessorLzxDecompress::~XChunkProcessorLzxDecompress()
{
    for (auto& lzxState : m_lzx_states)
        lzx_teardown(static_cast<struct lzx_state*>(lzxState));
}

size_t
    XChunkProcessorLzxDecompress::Process(unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    // mspack_system system{.open = nullptr,
    //                      .close = nullptr,
    //                      .read = nullptr,
    //                      .write = nullptr,
    //                      .seek = nullptr,
    //                      .tell = nullptr,
    //                      .message = nullptr,
    //                      .alloc = nullptr,
    //                      .free = nullptr,
    //                      .copy = nullptr,
    //                      .null_ptr = nullptr};
    // LzxInput input{.input = input, .inputLength = inputLength};
    // lzxd_init(&system, reinterpret_cast<mspack_file*>(&input), reinterpret_cast<mspack_file*>(&output), 15, 0, LZX_FRAME_SIZE * 4, 0, 0);
    // TODO: Implement

    auto* state = static_cast<struct lzx_state*>(m_lzx_states[streamNumber]);

    size_t curInputOffset = 0uz;
    size_t curInputSize = inputLength;

    size_t curOutputOffset = 0uz;
    size_t curOutputSize = outputBufferSize;

    uint8_t hi, lo;
    uint16_t dstSize, srcSize;

    while (curInputSize > 0)
    {
        hi = NextByte(input, curInputOffset, curInputSize);
        if (hi == 0xFF)
        {
            if (curInputSize < 4)
            {
                LogErrorHeaderSpace(curInputSize);
                return curOutputOffset;
            }

            hi = NextByte(input, curInputOffset, curInputSize);
            lo = NextByte(input, curInputOffset, curInputSize);
            dstSize = CombineHiLo(hi, lo);

            hi = NextByte(input, curInputOffset, curInputSize);
            lo = NextByte(input, curInputOffset, curInputSize);
            srcSize = CombineHiLo(hi, lo);
        }
        else
        {
            if (curInputSize < 1)
            {
                LogErrorHeaderSpace(curInputSize);
                return curOutputOffset;
            }

            dstSize = 0x8000u;
            lo = NextByte(input, curInputOffset, curInputSize);
            srcSize = CombineHiLo(hi, lo);
        }

        if (srcSize == 0 || dstSize == 0)
        {
            std::cerr << std::format("XMemCompress: EOF: {} {}, {}\n", srcSize, dstSize, curInputSize);
            return curOutputOffset;
        }

        if (srcSize > curInputSize)
        {
            std::cerr << std::format("XMemCompress: block size bigger than remaining data: {} > {}\n", srcSize, curInputSize);
            return curOutputOffset;
        }

        if (dstSize > curOutputSize)
        {
            std::cerr << std::format("XMemCompress: output size bigger than remaining data: {} > {}\n", dstSize, curOutputSize);
            return curOutputOffset;
        }

        std::cout << std::format("asdf {} = {} + {}\n", inputLength, curInputOffset, srcSize);
        auto ret = lzx_decompress(state, &input[curInputOffset], &output[curOutputOffset], srcSize, dstSize);
        if (ret == DECR_OK)
        {
            curInputOffset += srcSize;
            curInputSize -= srcSize;
            curOutputOffset += dstSize;
            curOutputSize -= srcSize;
        }

        std::cout << std::format("lzx: {}\n", ret);
    }

    // memcpy(output, input, inputLength);
    // return inputLength;
    return curOutputOffset;
}
