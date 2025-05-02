#include "StepLoadHash.h"

#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <cassert>

namespace
{
    class StepLoadHash final : public step::IStepLoadHash
    {
    public:
        StepLoadHash(const size_t hashSize, const unsigned hashCount)
            : m_hash_size(hashSize),
              m_hash_count(hashCount),
              m_hashes(std::make_unique<uint8_t[]>(hashSize * hashCount))
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            if (stream.Load(m_hashes.get(), m_hash_size * m_hash_count) != m_hash_size * m_hash_count)
                throw UnexpectedEndOfFileException();
        }

        void GetHash(const unsigned hashIndex, const uint8_t** pHash, size_t* pSize) override
        {
            assert(pHash != nullptr);
            assert(pSize != nullptr);
            assert(hashIndex < m_hash_count);

            assert(m_hashes);

            *pHash = &m_hashes[m_hash_size * hashIndex];
            *pSize = m_hash_size;
        }

        void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override
        {
            assert(pCapturedData != nullptr);
            assert(pSize != nullptr);

            *pCapturedData = m_hashes.get();
            *pSize = m_hash_size * m_hash_count;
        }

    private:
        size_t m_hash_size;
        unsigned m_hash_count;
        std::unique_ptr<uint8_t[]> m_hashes;
    };
} // namespace

namespace step
{
    std::unique_ptr<IStepLoadHash> CreateStepLoadHash(const size_t hashSize, const unsigned hashCount)
    {
        return std::make_unique<StepLoadHash>(hashSize, hashCount);
    }
} // namespace step
