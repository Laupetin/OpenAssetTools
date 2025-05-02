#include "ProcessorAuthedBlocks.h"

#include "Game/IW4/IW4.h"
#include "Loading/Exception/InvalidHashException.h"
#include "Loading/Exception/TooManyAuthedGroupsException.h"
#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <algorithm>
#include <cassert>
#include <memory>

class ProcessorAuthedBlocks final : public StreamProcessor
{
public:
    ProcessorAuthedBlocks(const unsigned authedChunkCount,
                          const size_t chunkSize,
                          const unsigned maxMasterBlockCount,
                          std::unique_ptr<cryptography::IHashFunction> hashFunction,
                          IHashProvider* masterBlockHashProvider)
        : m_authed_chunk_count(authedChunkCount),
          m_chunk_size(chunkSize),
          m_max_master_block_count(maxMasterBlockCount),
          m_hash_function(std::move(hashFunction)),
          m_master_block_hash_provider(masterBlockHashProvider),
          m_chunk_hashes_buffer(std::make_unique<uint8_t[]>(m_authed_chunk_count * m_hash_function->GetHashSize())),
          m_current_chunk_hash_buffer(std::make_unique<uint8_t[]>(m_hash_function->GetHashSize())),
          m_chunk_buffer(std::make_unique<uint8_t[]>(m_chunk_size)),
          m_current_group(1),
          m_current_chunk_in_group(0),
          m_current_chunk_offset(0),
          m_current_chunk_size(0)
    {
        assert(m_authed_chunk_count * m_hash_function->GetHashSize() <= m_chunk_size);
    }

    size_t Load(void* buffer, const size_t length) override
    {
        size_t loadedSize = 0;

        while (loadedSize < length)
        {
            if (m_current_chunk_offset >= m_current_chunk_size)
            {
                if (!NextChunk())
                    return loadedSize;
            }

            auto sizeToWrite = length - loadedSize;
            sizeToWrite = std::min(sizeToWrite, m_current_chunk_size - m_current_chunk_offset);

            assert(length - loadedSize >= sizeToWrite);
            memcpy(&static_cast<uint8_t*>(buffer)[loadedSize], &m_chunk_buffer[m_current_chunk_offset], sizeToWrite);
            loadedSize += sizeToWrite;
            m_current_chunk_offset += sizeToWrite;
        }

        return loadedSize;
    }

    int64_t Pos() override
    {
        return m_base_stream->Pos() - static_cast<int64_t>(m_current_chunk_size - m_current_chunk_offset);
    }

private:
    bool NextChunk()
    {
        m_current_chunk_offset = 0;

        while (true)
        {
            m_current_chunk_size = m_base_stream->Load(m_chunk_buffer.get(), m_chunk_size);

            if (m_current_chunk_size == 0)
                return false;

            m_hash_function->Init();
            m_hash_function->Process(m_chunk_buffer.get(), m_current_chunk_size);
            m_hash_function->Finish(m_current_chunk_hash_buffer.get());

            if (m_current_chunk_in_group == 0)
            {
                if (m_current_chunk_size < m_authed_chunk_count * m_hash_function->GetHashSize())
                    throw UnexpectedEndOfFileException();

                const uint8_t* masterBlockHash = nullptr;
                size_t masterBlockHashSize = 0;
                m_master_block_hash_provider->GetHash(m_current_group - 1, &masterBlockHash, &masterBlockHashSize);

                if (masterBlockHashSize != m_hash_function->GetHashSize()
                    || std::memcmp(m_current_chunk_hash_buffer.get(), masterBlockHash, m_hash_function->GetHashSize()) != 0)
                {
                    throw InvalidHashException();
                }

                memcpy(m_chunk_hashes_buffer.get(), m_chunk_buffer.get(), m_authed_chunk_count * m_hash_function->GetHashSize());

                m_current_chunk_in_group++;
            }
            else
            {
                if (std::memcmp(m_current_chunk_hash_buffer.get(),
                                &m_chunk_hashes_buffer[(m_current_chunk_in_group - 1) * m_hash_function->GetHashSize()],
                                m_hash_function->GetHashSize())
                    != 0)
                {
                    throw InvalidHashException();
                }

                if (++m_current_chunk_in_group > m_authed_chunk_count)
                {
                    m_current_chunk_in_group = 0;
                    m_current_group++;

                    if (m_current_group > m_max_master_block_count)
                        throw TooManyAuthedGroupsException();
                }

                return true;
            }
        }
    }

    const unsigned m_authed_chunk_count;
    const size_t m_chunk_size;
    const unsigned m_max_master_block_count;

    const std::unique_ptr<cryptography::IHashFunction> m_hash_function;
    IHashProvider* const m_master_block_hash_provider;
    const std::unique_ptr<uint8_t[]> m_chunk_hashes_buffer;
    const std::unique_ptr<uint8_t[]> m_current_chunk_hash_buffer;

    const std::unique_ptr<uint8_t[]> m_chunk_buffer;
    unsigned m_current_group;
    unsigned m_current_chunk_in_group;

    size_t m_current_chunk_offset;
    size_t m_current_chunk_size;
};

namespace processor
{
    std::unique_ptr<StreamProcessor> CreateProcessorAuthedBlocks(unsigned authedChunkCount,
                                                                 size_t chunkSize,
                                                                 unsigned maxMasterBlockCount,
                                                                 std::unique_ptr<cryptography::IHashFunction> hashFunction,
                                                                 IHashProvider* masterBlockHashProvider)
    {
        return std::make_unique<ProcessorAuthedBlocks>(authedChunkCount, chunkSize, maxMasterBlockCount, std::move(hashFunction), masterBlockHashProvider);
    }
} // namespace processor
