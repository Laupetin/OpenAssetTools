#include "ChunkProcessorSalsa20.h"
#include "Crypto.h"
#include <cassert>

class StreamContextSalsa20
{
public:
    IStreamCipher* m_salsa20;
    IHashFunction* m_sha1;

    StreamContextSalsa20()
    {
        m_salsa20 = nullptr;
        m_sha1 = nullptr;
    }

    ~StreamContextSalsa20()
    {
        delete m_salsa20;
        m_salsa20 = nullptr;

        delete m_sha1;
        m_sha1 = nullptr;
    }
};

class ChunkProcessorSalsa20::ChunkProcessorSalsa20Impl
{
    static const int BLOCK_HASHES_COUNT = 200;
    static const int SHA1_HASH_SIZE = 20;
    static const int SALSA20_IV_SIZE = 8;

    int m_stream_count;
    StreamContextSalsa20* m_stream_contexts;

    // m_block_hashes[BLOCK_HASHES_COUNT][numStreams][HASH_SIZE]
    uint8_t* m_block_hashes;
    unsigned int* m_stream_block_indices;

    uint8_t* GetHashBlock(const int streamNumber) const
    {
        const size_t blockIndexOffset = m_stream_block_indices[streamNumber] * m_stream_count * SHA1_HASH_SIZE;
        const size_t streamOffset = streamNumber * SHA1_HASH_SIZE;

        return &m_block_hashes[blockIndexOffset + streamOffset];
    }

public:
    ChunkProcessorSalsa20Impl(const int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize)
    {
        m_stream_count = streamCount;
        m_stream_contexts = new StreamContextSalsa20[streamCount];
        m_block_hashes = new uint8_t[BLOCK_HASHES_COUNT * streamCount * SHA1_HASH_SIZE];
        m_stream_block_indices = new unsigned int[streamCount];

        InitStreams(zoneName, salsa20Key, keySize);
    }

    ~ChunkProcessorSalsa20Impl()
    {
        delete[] m_stream_contexts;

        delete[] m_block_hashes;
        m_block_hashes = nullptr;

        delete[] m_stream_block_indices;
        m_stream_block_indices = nullptr;
    }

    void InitStreams(std::string& zoneName, const uint8_t* salsa20Key, size_t keySize) const
    {
        const int zoneNameLength = zoneName.length();
        const size_t blockHashBufferSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;

        assert(blockHashBufferSize % 4 == 0);

        size_t zoneNameOffset = 0;
        for(size_t i = 0; i < blockHashBufferSize; i += 4)
        {
            *reinterpret_cast<uint32_t*>(&m_block_hashes[i]) = 0x1010101 * zoneName[zoneNameOffset++];

            zoneNameOffset %= zoneNameLength;
        }

        for(int stream = 0; stream < m_stream_count; stream++)
        {
            m_stream_block_indices[stream] = 0;

            m_stream_contexts[stream].m_salsa20 = Crypto::CreateSalsa20(salsa20Key, keySize);
            m_stream_contexts[stream].m_sha1 = Crypto::CreateSHA1();
        }
    }

    size_t Process(const int streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize) const
    {
        assert(streamNumber >= 0 && streamNumber < m_stream_count);
        assert(input != nullptr);
        assert(output != nullptr);
        assert(inputLength <= outputBufferSize);

        StreamContextSalsa20& streamContext = m_stream_contexts[streamNumber];

        // Initialize Salsa20 with an IV of the first 8 bytes of the current hash block
        streamContext.m_salsa20->SetIV(GetHashBlock(streamNumber), SALSA20_IV_SIZE);
        streamContext.m_salsa20->Process(input, output, inputLength);

        // Hash decrypted XChunk
        uint8_t blockSha1Hash[SHA1_HASH_SIZE];
        streamContext.m_sha1->Init();
        streamContext.m_sha1->Process(output, inputLength);
        streamContext.m_sha1->Finish(&blockSha1Hash);

        // Advance index to next hash block
        m_stream_block_indices[streamNumber] = (m_stream_block_indices[streamNumber] + 1) % BLOCK_HASHES_COUNT;

        uint8_t* nextHashBlock = GetHashBlock(streamNumber);

        // XOR the upcoming hash block with the hash of the XChunk utilizing the previous hash block
        for(unsigned int hashOffset = 0; hashOffset < sizeof(blockSha1Hash); hashOffset++)
        {
            nextHashBlock[hashOffset] ^= blockSha1Hash[hashOffset];
        }

        return inputLength;
    }

    void GetSignatureData(const uint8_t** pSignatureData, size_t* pSize) const
    {
        assert(pSignatureData != nullptr);
        assert(pSize != nullptr);

        *pSignatureData = m_block_hashes;
        *pSize = BLOCK_HASHES_COUNT * m_stream_count * SHA1_HASH_SIZE;
    }
};

ChunkProcessorSalsa20::ChunkProcessorSalsa20(const int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize)
{
    m_impl = new ChunkProcessorSalsa20Impl(streamCount, zoneName, salsa20Key, keySize);
}

ChunkProcessorSalsa20::~ChunkProcessorSalsa20()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t ChunkProcessorSalsa20::Process(const int streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    return m_impl->Process(streamNumber, input, inputLength, output, outputBufferSize);
}

void ChunkProcessorSalsa20::GetCapturedData(const uint8_t** pCapturedData, size_t* pSize)
{
    m_impl->GetSignatureData(pCapturedData, pSize);
}