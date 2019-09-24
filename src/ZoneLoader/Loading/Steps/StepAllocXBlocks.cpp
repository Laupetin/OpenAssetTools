#include "StepAllocXBlocks.h"
#include "Loading/Exception/InvalidXBlockSizeException.h"

const uint64_t StepAllocXBlocks::MAX_XBLOCK_SIZE = 0x3C000000;

void StepAllocXBlocks::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    const unsigned int blockCount = zoneLoader->m_blocks.size();

    auto* blockSizes = new xblock_size_t[blockCount];
    stream->Load(blockSizes, sizeof(xblock_size_t) * blockCount);

    uint64_t totalMemory = 0;
    for(unsigned int block = 0; block < blockCount; block++)
    {
        totalMemory += blockSizes[block];
    }

    if(totalMemory > MAX_XBLOCK_SIZE)
    {
        throw InvalidXBlockSizeException(totalMemory, MAX_XBLOCK_SIZE);
    }

    for(unsigned int block = 0; block < blockCount; block++)
    {
        zoneLoader->m_blocks[block]->Alloc(blockSizes[block]);
    }

    delete[] blockSizes;
}