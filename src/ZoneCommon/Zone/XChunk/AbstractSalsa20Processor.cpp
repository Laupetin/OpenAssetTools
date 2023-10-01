#include "AbstractSalsa20Processor.h"

#include <cassert>

AbstractSalsa20Processor::AbstractSalsa20Processor(const int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize)
    : m_stream_count(streamCount),
      m_stream_contexts(std::make_unique<StreamContext[]>(streamCount)),
      m_stream_block_indices(std::make_unique<unsigned int[]>(streamCount))
{
    m_block_hashes = std::make_unique<uint8_t[]>(BLOCK_HASHES_COUNT * streamCount * SHA1_HASH_SIZE);
    InitStreams(zoneName, salsa20Key, keySize);
}

uint8_t* AbstractSalsa20Processor::GetHashBlock(const int streamNumber) const
{
    const auto blockIndexOffset = m_stream_block_indices[streamNumber] * m_stream_count * SHA1_HASH_SIZE;
    const auto streamOffset = static_cast<size_t>(streamNumber) * SHA1_HASH_SIZE;

    return &m_block_hashes[blockIndexOffset + streamOffset];
}

void AbstractSalsa20Processor::InitStreams(std::string& zoneName, const uint8_t* salsa20Key, size_t keySize) const
{
    const int zoneNameLength = zoneName.length();
    const size_t blockHashBufferSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;

    assert(blockHashBufferSize % 4 == 0);

    size_t zoneNameOffset = 0;
    for (size_t i = 0; i < blockHashBufferSize; i += 4)
    {
        *reinterpret_cast<uint32_t*>(&m_block_hashes[i]) = 0x1010101 * zoneName[zoneNameOffset++];

        zoneNameOffset %= zoneNameLength;
    }

    for (auto stream = 0; stream < m_stream_count; stream++)
    {
        m_stream_block_indices[stream] = 0;

        m_stream_contexts[stream].m_salsa20 = Crypto::CreateSalsa20(salsa20Key, keySize);
        m_stream_contexts[stream].m_sha1 = Crypto::CreateSHA1();
    }
}

void AbstractSalsa20Processor::GetCapturedData(const uint8_t** pCapturedData, size_t* pSize)
{
    assert(pCapturedData != nullptr);
    assert(pSize != nullptr);

    *pCapturedData = m_block_hashes.get();
    *pSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;
}
