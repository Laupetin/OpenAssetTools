#include "XChunkProcessorInflate.h"

#include "XChunkException.h"

#include <format>
#include <iostream>
#include <zlib.h>
#include <zutil.h>

size_t XChunkProcessorInflate::Process(unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    auto ret = inflateInit2(&stream, -DEF_WBITS);
    if (ret != Z_OK)
        throw XChunkException("Initializing inflate failed.");

    stream.avail_in = static_cast<unsigned>(inputLength);
    stream.next_in = input;
    stream.avail_out = static_cast<unsigned>(outputBufferSize);
    stream.next_out = output;

    ret = inflate(&stream, Z_FULL_FLUSH);
    if (ret != Z_STREAM_END)
    {
        std::cerr << std::format("inflate of stream failed with error code {}: {}\n", streamNumber, ret, stream.msg);
        throw XChunkException(std::format("Zone has invalid or unsupported compression: {}", stream.msg));
    }

    const size_t outputSize = stream.total_out;

    inflateEnd(&stream);

    return outputSize;
}
