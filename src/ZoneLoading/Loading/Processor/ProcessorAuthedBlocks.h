#pragma once

#include "Cryptography.h"
#include "Loading/IHashProvider.h"
#include "Loading/StreamProcessor.h"

#include <memory>

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorAuthedBlocks(unsigned authedChunkCount,
                                                                 size_t chunkSize,
                                                                 unsigned maxMasterBlockCount,
                                                                 std::unique_ptr<cryptography::IHashFunction> hashFunction,
                                                                 IHashProvider* masterBlockHashProvider);
}
