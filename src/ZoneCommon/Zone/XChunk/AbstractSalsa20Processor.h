#pragma once

#include "Cryptography.h"
#include "Utils/ICapturedDataProvider.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Salsa20StreamContext
{
public:
    std::unique_ptr<cryptography::IStreamCipher> m_salsa20;
    std::unique_ptr<cryptography::IHashFunction> m_sha1;
};

class AbstractSalsa20Processor : public ICapturedDataProvider
{
public:
    virtual ~AbstractSalsa20Processor() = default;
    AbstractSalsa20Processor(const AbstractSalsa20Processor& other) = delete;
    AbstractSalsa20Processor(AbstractSalsa20Processor&& other) noexcept = default;
    AbstractSalsa20Processor& operator=(const AbstractSalsa20Processor& other) = delete;
    AbstractSalsa20Processor& operator=(AbstractSalsa20Processor&& other) noexcept = default;

    void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override;

protected:
    static constexpr auto BLOCK_HASHES_COUNT = 200u;
    static constexpr auto SHA1_HASH_SIZE = 20u;
    static constexpr auto SALSA20_IV_SIZE = 8u;

    AbstractSalsa20Processor(unsigned streamCount, const std::string& zoneName, const uint8_t* salsa20Key, unsigned keySize);
    void InitStreams(const std::string& zoneName, const uint8_t* salsa20Key, size_t keySize);

    [[nodiscard]] uint8_t* GetHashBlock(unsigned streamNumber);

    unsigned m_stream_count;
    std::vector<Salsa20StreamContext> m_stream_contexts;

    // m_block_hashes[BLOCK_HASHES_COUNT][numStreams][SHA1_HASH_SIZE]
    std::vector<uint8_t> m_block_hashes;
    std::vector<unsigned int> m_stream_block_indices;
};
