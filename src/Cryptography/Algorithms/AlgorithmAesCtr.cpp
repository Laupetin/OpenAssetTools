#include "AlgorithmAesCtr.h"

#include "tomcrypt.h"

#include <cassert>
#include <stdexcept>
#include <vector>

using namespace cryptography;

namespace
{
    constexpr size_t AES_BLOCK_SIZE = 16;

    class AlgorithmAesCtr final : public IStreamCipher
    {
    public:
        AlgorithmAesCtr(const uint8_t* keyBytes, const size_t keySize)
            : m_key(keyBytes, keyBytes + keySize),
              m_ctr{},
              m_started(false)
        {
            if (register_cipher(&aes_desc) == -1)
                throw std::runtime_error("Failed to register AES cipher");

            m_cipher_index = find_cipher("aes");
            if (m_cipher_index == -1)
                throw std::runtime_error("AES cipher is not available");
        }

        ~AlgorithmAesCtr() override
        {
            if (m_started)
                ctr_done(&m_ctr);
        }

        AlgorithmAesCtr(const AlgorithmAesCtr& other) = delete;
        AlgorithmAesCtr(AlgorithmAesCtr&& other) noexcept = delete;
        AlgorithmAesCtr& operator=(const AlgorithmAesCtr& other) = delete;
        AlgorithmAesCtr& operator=(AlgorithmAesCtr&& other) noexcept = delete;

        void SetIv(const uint8_t* iv, const size_t ivSize) override
        {
            assert(ivSize == AES_BLOCK_SIZE);

            if (ivSize != AES_BLOCK_SIZE)
                throw std::invalid_argument("AES CTR IV size must be 16");

            if (m_started)
            {
                ctr_done(&m_ctr);
                m_started = false;
            }

            if (ctr_start(m_cipher_index, iv, m_key.data(), static_cast<int>(m_key.size()), 0, CTR_COUNTER_LITTLE_ENDIAN, &m_ctr) != CRYPT_OK)
                throw std::invalid_argument("Failed to start AES CTR with the specified key");

            m_started = true;
        }

        void Process(const void* plainText, void* cipherText, const size_t amount) override
        {
            if (!m_started)
                throw std::logic_error("AES CTR IV was not set");

            ctr_encrypt(static_cast<const unsigned char*>(plainText), static_cast<unsigned char*>(cipherText), static_cast<unsigned long>(amount), &m_ctr);
        }

    private:
        std::vector<uint8_t> m_key;
        symmetric_CTR m_ctr;
        int m_cipher_index;
        bool m_started;
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IStreamCipher> CreateAesCtr(const uint8_t* keyBytes, const size_t keySize)
    {
        return std::make_unique<AlgorithmAesCtr>(keyBytes, keySize);
    }
} // namespace cryptography
