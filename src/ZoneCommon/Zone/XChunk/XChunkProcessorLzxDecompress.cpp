#include "XChunkProcessorLzxDecompress.h"

#include <cstring>

size_t
    XChunkProcessorLzxDecompress::Process(unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    // TODO: Implement

    memcpy(output, input, inputLength);
    return inputLength;
}
