#include "XChunkProcessorSalsa20Decryption.h"

#include "AbstractSalsa20Processor.h"
#include "Cryptography.h"

#include <cassert>
#include <format>
#include <iostream>

XChunkProcessorSalsa20Decryption::XChunkProcessorSalsa20Decryption(const unsigned streamCount,
                                                                   const std::string& zoneName,
                                                                   const uint8_t* salsa20Key,
                                                                   const size_t keySize)
    : AbstractSalsa20Processor(streamCount, zoneName, salsa20Key, keySize)
{
}

size_t XChunkProcessorSalsa20Decryption::Process(
    const unsigned streamNumber, const uint8_t* input, const size_t inputLength, uint8_t* output, const size_t outputBufferSize)
{
    assert(streamNumber < m_stream_count);
    assert(input != nullptr);
    assert(output != nullptr);
    assert(inputLength <= outputBufferSize);

    const auto& streamContext = m_stream_contexts[streamNumber];

    // Initialize Salsa20 with an IV of the first 8 bytes of the current hash block
    streamContext.m_salsa20->SetIv(GetHashBlock(streamNumber), SALSA20_IV_SIZE);
    streamContext.m_salsa20->Process(input, output, inputLength);

    // Hash decrypted XChunk
    uint8_t blockSha1Hash[SHA1_HASH_SIZE];
    streamContext.m_sha1->Init();
    streamContext.m_sha1->Process(output, inputLength);
    streamContext.m_sha1->Finish(&blockSha1Hash);

    // Advance index to next hash block
    m_stream_block_indices[streamNumber] = (m_stream_block_indices[streamNumber] + 1) % BLOCK_HASHES_COUNT;

    auto* nextHashBlock = GetHashBlock(streamNumber);

    // XOR the upcoming hash block with the hash of the XChunk utilizing the previous hash block
    for (unsigned int hashOffset = 0; hashOffset < sizeof(blockSha1Hash); hashOffset++)
    {
        nextHashBlock[hashOffset] ^= blockSha1Hash[hashOffset];
    }

    return inputLength;
}
