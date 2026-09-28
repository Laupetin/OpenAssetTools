#include "ProcessorSkipAuthedBlocks.h"

#include "Loading/Exception/TooManyAuthedGroupsException.h"
#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>

namespace
{
    class ProcessorSkipAuthedBlocks final : public StreamProcessor
    {
    public:
        ProcessorSkipAuthedBlocks(const unsigned authedChunkCount, const size_t chunkSize, const unsigned maxMasterBlockCount, const size_t hashSize)
            : m_authed_chunk_count(authedChunkCount),
              m_chunk_size(chunkSize),
              m_max_master_block_count(maxMasterBlockCount),
              m_hash_size(hashSize),
              m_chunk_buffer(std::make_unique<uint8_t[]>(m_chunk_size)),
              m_current_group(1),
              m_current_chunk_in_group(0),
              m_current_chunk_offset(0),
              m_current_chunk_size(0)
        {
            assert(m_authed_chunk_count * hashSize <= m_chunk_size);
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
                std::memcpy(&static_cast<uint8_t*>(buffer)[loadedSize], &m_chunk_buffer[m_current_chunk_offset], sizeToWrite);
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

                if (m_current_chunk_in_group == 0)
                {
                    if (m_current_chunk_size < m_authed_chunk_count * m_hash_size)
                        throw UnexpectedEndOfFileException();

                    m_current_chunk_in_group++;
                }
                else
                {
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

        const size_t m_hash_size;

        const std::unique_ptr<uint8_t[]> m_chunk_buffer;
        unsigned m_current_group;
        unsigned m_current_chunk_in_group;

        size_t m_current_chunk_offset;
        size_t m_current_chunk_size;
    };
} // namespace

namespace processor
{
    std::unique_ptr<StreamProcessor>
        CreateProcessorSkipAuthedBlocks(const unsigned authedChunkCount, const size_t chunkSize, const unsigned maxMasterBlockCount, const size_t hashSize)
    {
        return std::make_unique<ProcessorSkipAuthedBlocks>(authedChunkCount, chunkSize, maxMasterBlockCount, hashSize);
    }
} // namespace processor
