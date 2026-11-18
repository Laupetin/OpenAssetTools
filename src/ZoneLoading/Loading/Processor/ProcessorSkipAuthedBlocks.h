#pragma once

#include "Loading/StreamProcessor.h"

#include <memory>

namespace processor
{
    std::unique_ptr<StreamProcessor>
        CreateProcessorSkipAuthedBlocks(unsigned authedChunkCount, size_t chunkSize, unsigned maxMasterBlockCount, size_t hashSize);
}
