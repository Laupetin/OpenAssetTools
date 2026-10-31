#include "ProcessorAuthedBlocksCODO.h"

#include "Cryptography.h"
#include "Game/CODO/ZoneConstantsCODO.h"
#include "Loading/Exception/InvalidHashException.h"
#include "Loading/Exception/InvalidMagicException.h"
#include "Loading/Exception/InvalidSignatureException.h"
#include "Loading/Exception/TooManyAuthedGroupsException.h"
#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <algorithm>
#include <cassert>
#include <cstring>

using namespace CODO;

namespace
{
    constexpr size_t RSA_BLOCK_SIZE = 256;
    constexpr size_t HASH_SIZE = 32;
    constexpr size_t AUTH_HEADER_MASTER_HASHES_OFFSET = 0x150;
    constexpr unsigned AUTH_HEADER_MASTER_HASH_COUNT = 244;

    static_assert(ZoneConstants::AUTHED_CHUNK_COUNT_PER_GROUP * HASH_SIZE == ZoneConstants::AUTHED_CHUNK_SIZE);
    static_assert(AUTH_HEADER_MASTER_HASHES_OFFSET + AUTH_HEADER_MASTER_HASH_COUNT * HASH_SIZE <= ZoneConstants::AUTHED_CHUNK_SIZE);

    class StepLoadZoneCipherKey final : public ILoadingStep
    {
    public:
        explicit StepLoadZoneCipherKey(std::shared_ptr<ZoneCipherKey> cipherKey)
            : m_cipher_key(std::move(cipherKey))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            uint8_t encrypted[RSA_BLOCK_SIZE];
            if (stream.Load(encrypted, sizeof(encrypted)) != sizeof(encrypted))
                throw UnexpectedEndOfFileException();

            uint8_t decrypted[RSA_BLOCK_SIZE];
            auto decryptedSize = sizeof(decrypted);
            if (!cryptography::RsaDecryptPkcs1(
                    ZoneConstants::RSA_KEY_UNWRAP, sizeof(ZoneConstants::RSA_KEY_UNWRAP), encrypted, sizeof(encrypted), decrypted, decryptedSize)
                || decryptedSize != AES_KEY_SIZE + AES_IV_SIZE)
            {
                throw InvalidSignatureException();
            }

            std::memcpy(m_cipher_key->m_key.data(), decrypted, AES_KEY_SIZE);
            std::memcpy(m_cipher_key->m_iv.data(), &decrypted[AES_KEY_SIZE], AES_IV_SIZE);
        }

    private:
        std::shared_ptr<ZoneCipherKey> m_cipher_key;
    };

    class ProcessorAuthedBlocks final : public StreamProcessor
    {
    public:
        explicit ProcessorAuthedBlocks(std::shared_ptr<ZoneCipherKey> cipherKey)
            : m_cipher_key(std::move(cipherKey)),
              m_hash_function(cryptography::CreateSha256()),
              m_master_hashes(AUTH_HEADER_MASTER_HASH_COUNT * HASH_SIZE),
              m_chunk_hashes(ZoneConstants::AUTHED_CHUNK_COUNT_PER_GROUP * HASH_SIZE),
              m_chunk_buffer(ZoneConstants::AUTHED_CHUNK_SIZE),
              m_initialized(false),
              m_current_group(0),
              m_current_chunk_in_group(0),
              m_current_chunk_offset(0),
              m_current_chunk_size(0)
        {
            assert(m_hash_function->GetHashSize() == HASH_SIZE);
        }

        size_t Load(void* buffer, const size_t length) override
        {
            if (!m_initialized)
                Initialize();

            size_t loadedSize = 0;

            while (loadedSize < length)
            {
                if (m_current_chunk_offset >= m_current_chunk_size)
                {
                    if (!NextChunk())
                        return loadedSize;
                }

                const auto sizeToWrite = std::min(length - loadedSize, m_current_chunk_size - m_current_chunk_offset);

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
        void DecryptChunk(const uint8_t* iv)
        {
            auto cipher = cryptography::CreateAesCtr(m_cipher_key->m_key.data(), m_cipher_key->m_key.size());
            cipher->SetIv(iv, AES_IV_SIZE);
            cipher->Process(m_chunk_buffer.data(), m_chunk_buffer.data(), m_current_chunk_size);
        }

        [[nodiscard]] bool MatchesHash(const uint8_t* expectedHash)
        {
            uint8_t hash[HASH_SIZE];

            m_hash_function->Init();
            m_hash_function->Process(m_chunk_buffer.data(), m_current_chunk_size);
            m_hash_function->Finish(hash);

            return std::memcmp(hash, expectedHash, HASH_SIZE) == 0;
        }

        void Initialize()
        {
            m_initialized = true;

            // The first chunk is the auth header which is only used for the master hashes
            m_current_chunk_size = m_base_stream->Load(m_chunk_buffer.data(), m_chunk_buffer.size());
            if (m_current_chunk_size < m_chunk_buffer.size())
                throw UnexpectedEndOfFileException();

            DecryptChunk(m_cipher_key->m_iv.data());

            if (std::memcmp(m_chunk_buffer.data(), ZoneConstants::MAGIC_AUTH_HEADER, std::char_traits<char>::length(ZoneConstants::MAGIC_AUTH_HEADER)) != 0)
                throw InvalidMagicException(ZoneConstants::MAGIC_AUTH_HEADER);

            std::memcpy(m_master_hashes.data(), &m_chunk_buffer[AUTH_HEADER_MASTER_HASHES_OFFSET], m_master_hashes.size());

            m_current_chunk_size = 0;
            m_current_chunk_offset = 0;
        }

        bool NextChunk()
        {
            m_current_chunk_offset = 0;

            while (true)
            {
                m_current_chunk_size = m_base_stream->Load(m_chunk_buffer.data(), m_chunk_buffer.size());

                if (m_current_chunk_size == 0)
                    return false;

                if (m_current_chunk_in_group == 0)
                {
                    // Hash table chunk which contains the hashes of the data chunks that follow
                    if (m_current_group >= AUTH_HEADER_MASTER_HASH_COUNT)
                        throw TooManyAuthedGroupsException();

                    if (m_current_chunk_size < m_chunk_hashes.size())
                        throw UnexpectedEndOfFileException();

                    const auto* masterHash = &m_master_hashes[m_current_group * HASH_SIZE];

                    DecryptChunk(masterHash);
                    if (!MatchesHash(masterHash))
                        throw InvalidHashException();

                    std::memcpy(m_chunk_hashes.data(), m_chunk_buffer.data(), m_chunk_hashes.size());
                    m_current_chunk_in_group++;
                }
                else
                {
                    const auto* chunkHash = &m_chunk_hashes[(m_current_chunk_in_group - 1) * HASH_SIZE];

                    DecryptChunk(chunkHash);
                    if (!MatchesHash(chunkHash))
                        throw InvalidHashException();

                    if (++m_current_chunk_in_group > ZoneConstants::AUTHED_CHUNK_COUNT_PER_GROUP)
                    {
                        m_current_chunk_in_group = 0;
                        m_current_group++;
                    }

                    return true;
                }
            }
        }

        std::shared_ptr<ZoneCipherKey> m_cipher_key;
        std::unique_ptr<cryptography::IHashFunction> m_hash_function;

        std::vector<uint8_t> m_master_hashes;
        std::vector<uint8_t> m_chunk_hashes;
        std::vector<uint8_t> m_chunk_buffer;

        bool m_initialized;
        unsigned m_current_group;
        unsigned m_current_chunk_in_group;

        size_t m_current_chunk_offset;
        size_t m_current_chunk_size;
    };
} // namespace

namespace CODO
{
    std::unique_ptr<ILoadingStep> CreateStepLoadZoneCipherKey(std::shared_ptr<ZoneCipherKey> cipherKey)
    {
        return std::make_unique<StepLoadZoneCipherKey>(std::move(cipherKey));
    }

    std::unique_ptr<StreamProcessor> CreateProcessorAuthedBlocks(std::shared_ptr<ZoneCipherKey> cipherKey)
    {
        return std::make_unique<ProcessorAuthedBlocks>(std::move(cipherKey));
    }
} // namespace CODO
