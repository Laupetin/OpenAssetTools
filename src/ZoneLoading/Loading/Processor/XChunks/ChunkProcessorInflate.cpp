#include "ChunkProcessorInflate.h"

#include <stdexcept>

#include "zlib.h"
#include "zutil.h"

#include "Loading/Exception/InvalidCompressionException.h"

size_t ChunkProcessorInflate::Process(int streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    z_stream stream{};
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    int ret = inflateInit2(&stream, -DEF_WBITS);
    if(ret != Z_OK)
    {
        throw std::runtime_error("Initializing inflate failed.");
    }

    stream.avail_in = inputLength;
    stream.next_in = input;
    stream.avail_out = outputBufferSize;
    stream.next_out = output;

    ret = inflate(&stream, Z_FULL_FLUSH);
    if(ret != Z_STREAM_END)
    {
        throw InvalidCompressionException();
    }

    const size_t outputSize = stream.total_out;

    inflateEnd(&stream);

    return outputSize;
}
