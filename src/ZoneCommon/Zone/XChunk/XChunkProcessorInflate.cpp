#include "XChunkProcessorInflate.h"

#include "XChunkException.h"

#include <zlib.h>
#include <zutil.h>

size_t XChunkProcessorInflate::Process(int streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    auto ret = inflateInit2(&stream, -DEF_WBITS);
    if (ret != Z_OK)
        throw XChunkException("Initializing inflate failed.");

    stream.avail_in = inputLength;
    stream.next_in = input;
    stream.avail_out = outputBufferSize;
    stream.next_out = output;

    ret = inflate(&stream, Z_FULL_FLUSH);
    if (ret != Z_STREAM_END)
        throw XChunkException("Zone has invalid or unsupported compression. Inflate failed");

    const size_t outputSize = stream.total_out;

    inflateEnd(&stream);

    return outputSize;
}
