#include "XChunkProcessorDeflate.h"

#include "XChunkException.h"

#include <cassert>
#include <zlib.h>
#include <zutil.h>

size_t XChunkProcessorDeflate::Process(int streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    auto ret = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, -DEF_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        throw XChunkException("Initializing deflate failed.");

    stream.avail_in = static_cast<unsigned>(inputLength);
    stream.next_in = input;
    stream.avail_out = static_cast<unsigned>(outputBufferSize);
    stream.next_out = output;

    ret = deflate(&stream, Z_FINISH);
    if (ret != Z_STREAM_END)
        throw XChunkException("Failed to deflate memory of zone.");

    const size_t outputSize = stream.total_out;

    deflateEnd(&stream);

    return outputSize;
}
