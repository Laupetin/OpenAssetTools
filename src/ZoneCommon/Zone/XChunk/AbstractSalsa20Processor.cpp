#include "AbstractSalsa20Processor.h"

#include <cassert>

AbstractSalsa20Processor::AbstractSalsa20Processor(const unsigned streamCount, const std::string& zoneName, const uint8_t* salsa20Key, const unsigned keySize)
    : m_stream_count(streamCount),
      m_stream_contexts(streamCount),
      m_block_hashes(BLOCK_HASHES_COUNT * streamCount * SHA1_HASH_SIZE),
      m_stream_block_indices(streamCount)
{
    InitStreams(zoneName, salsa20Key, keySize);
}

void AbstractSalsa20Processor::InitStreams(const std::string& zoneName, const uint8_t* salsa20Key, const size_t keySize)
{
    // Original buffer must have been 32 bytes because the zoneName can at most be 31 characters be long before being cut off
    const auto zoneNameLength = std::min(zoneName.length(), 31uz);

    const size_t blockHashBufferSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;

    assert(blockHashBufferSize % 4 == 0);

    size_t zoneNameOffset = 0;
    for (auto i = 0uz; i < blockHashBufferSize; i += 4)
    {
        memset(&m_block_hashes[i], zoneName[zoneNameOffset++], 4u);

        zoneNameOffset %= zoneNameLength;
    }

    for (auto stream = 0u; stream < m_stream_count; stream++)
    {
        m_stream_block_indices[stream] = 0;

        m_stream_contexts[stream].m_salsa20 = cryptography::CreateSalsa20(salsa20Key, keySize);
        m_stream_contexts[stream].m_sha1 = cryptography::CreateSha1();
    }
}

uint8_t* AbstractSalsa20Processor::GetHashBlock(const unsigned streamNumber)
{
    const auto blockIndexOffset = m_stream_block_indices[streamNumber] * m_stream_count * SHA1_HASH_SIZE;
    const auto streamOffset = static_cast<size_t>(streamNumber) * SHA1_HASH_SIZE;

    return &m_block_hashes[blockIndexOffset + streamOffset];
}

void AbstractSalsa20Processor::GetCapturedData(const uint8_t** pCapturedData, size_t* pSize)
{
    assert(pCapturedData != nullptr);
    assert(pSize != nullptr);

    *pCapturedData = m_block_hashes.data();
    *pSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;
}
