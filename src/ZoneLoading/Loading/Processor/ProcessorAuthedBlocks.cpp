#include "ProcessorAuthedBlocks.h"

class ProcessorAuthedBlocks::Impl final : public StreamProcessor
{
    const int m_authed_chunk_count;
    const int m_max_master_block_count;
    IHashProvider* m_hash_provider;

public:
    Impl(const int authedChunkCount, const int maxMasterBlockCount, IHashProvider* masterBlockHashProvider)
        : m_authed_chunk_count(authedChunkCount),
          m_max_master_block_count(maxMasterBlockCount),
          m_hash_provider(masterBlockHashProvider)
    {
    }

    size_t Load(void* buffer, size_t length) override
    {
        return 0;
    }

    int64_t Pos() override
    {
        return 0;
    }
};

ProcessorAuthedBlocks::ProcessorAuthedBlocks(const int authedChunkCount, const int maxMasterBlockCount, IHashProvider* masterBlockHashProvider)
    : m_impl(new Impl(authedChunkCount, maxMasterBlockCount, masterBlockHashProvider))
{
}

ProcessorAuthedBlocks::~ProcessorAuthedBlocks()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t ProcessorAuthedBlocks::Load(void* buffer, const size_t length)
{
    return m_impl->Load(buffer, length);
}

int64_t ProcessorAuthedBlocks::Pos()
{
    return m_impl->Pos();
}
