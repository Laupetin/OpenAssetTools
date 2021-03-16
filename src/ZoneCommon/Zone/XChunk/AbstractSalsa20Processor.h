#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include "Utils/ClassUtils.h"
#include "Crypto.h"
#include "Utils/ICapturedDataProvider.h"

class AbstractSalsa20Processor : public ICapturedDataProvider
{
protected:
    static constexpr int BLOCK_HASHES_COUNT = 200;
    static constexpr int SHA1_HASH_SIZE = 20;
    static constexpr int SALSA20_IV_SIZE = 8;

    class StreamContext
    {
    public:
        std::unique_ptr<IStreamCipher> m_salsa20;
        std::unique_ptr<IHashFunction> m_sha1;
    };

    int m_stream_count;
    std::unique_ptr<StreamContext[]> m_stream_contexts;

    // m_block_hashes[BLOCK_HASHES_COUNT][numStreams][HASH_SIZE]
    std::unique_ptr<uint8_t[]> m_block_hashes;
    std::unique_ptr<unsigned int[]> m_stream_block_indices;

    AbstractSalsa20Processor(int streamCount, std::string& zoneName, const uint8_t* salsa20Key, size_t keySize);

    _NODISCARD uint8_t* GetHashBlock(int streamNumber) const;

    void InitStreams(std::string& zoneName, const uint8_t* salsa20Key, size_t keySize) const;

public:
    virtual ~AbstractSalsa20Processor() = default;
    AbstractSalsa20Processor(const AbstractSalsa20Processor& other) = delete;
    AbstractSalsa20Processor(AbstractSalsa20Processor&& other) noexcept = default;
    AbstractSalsa20Processor& operator=(const AbstractSalsa20Processor& other) = delete;
    AbstractSalsa20Processor& operator=(AbstractSalsa20Processor&& other) noexcept = default;

    void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override;
};
