#pragma once
#include "Loading/StreamProcessor.h"
#include "Loading/IHashProvider.h"

class ProcessorAuthedBlocks : public StreamProcessor
{
    class Impl;
    Impl* m_impl;

public:
    ProcessorAuthedBlocks(int authedChunkCount, int maxMasterBlockCount, IHashProvider* masterBlockHashProvider);
    ~ProcessorAuthedBlocks() override;

    size_t Load(void* buffer, size_t length) override;
    int64_t Pos() override;
};
