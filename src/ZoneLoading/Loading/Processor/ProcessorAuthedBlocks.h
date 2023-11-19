#pragma once
#include "Crypto.h"
#include "Loading/IHashProvider.h"
#include "Loading/StreamProcessor.h"

#include <memory>

class ProcessorAuthedBlocks final : public StreamProcessor
{
    class Impl;
    Impl* m_impl;

public:
    ProcessorAuthedBlocks(unsigned authedChunkCount,
                          size_t chunkSize,
                          unsigned maxMasterBlockCount,
                          std::unique_ptr<IHashFunction> hashFunction,
                          IHashProvider* masterBlockHashProvider);
    ~ProcessorAuthedBlocks() override;
    ProcessorAuthedBlocks(const ProcessorAuthedBlocks& other) = delete;
    ProcessorAuthedBlocks(ProcessorAuthedBlocks&& other) noexcept = default;
    ProcessorAuthedBlocks& operator=(const ProcessorAuthedBlocks& other) = delete;
    ProcessorAuthedBlocks& operator=(ProcessorAuthedBlocks&& other) noexcept = default;

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
