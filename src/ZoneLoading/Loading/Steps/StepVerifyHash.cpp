#include "StepVerifyHash.h"

#include "Loading/Exception/InvalidHashException.h"

#include <cstring>
#include <memory>

namespace
{
    class StepVerifyHash final : public ILoadingStep
    {
    public:
        StepVerifyHash(std::unique_ptr<cryptography::IHashFunction> hashFunction,
                       const unsigned hashIndex,
                       IHashProvider* hashProvider,
                       ICapturedDataProvider* dataProvider)
            : m_hash_function(std::move(hashFunction)),
              m_hash_index(hashIndex),
              m_hash_provider(hashProvider),
              m_data_provider(dataProvider)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            const uint8_t* dataToHash = nullptr;
            size_t dataToHashSize = 0;
            m_data_provider->GetCapturedData(&dataToHash, &dataToHashSize);

            const uint8_t* hashData = nullptr;
            size_t hashSize = 0;
            m_hash_provider->GetHash(m_hash_index, &hashData, &hashSize);

            if (hashSize != m_hash_function->GetHashSize())
                throw InvalidHashException();

            const auto hashMemory = std::make_unique<uint8_t[]>(m_hash_function->GetHashSize());
            m_hash_function->Init();
            m_hash_function->Process(dataToHash, dataToHashSize);
            m_hash_function->Finish(hashMemory.get());

            if (std::memcmp(hashData, hashMemory.get(), m_hash_function->GetHashSize()) != 0)
                throw InvalidHashException();
        }

    private:
        std::unique_ptr<cryptography::IHashFunction> m_hash_function;
        unsigned m_hash_index;
        IHashProvider* m_hash_provider;
        ICapturedDataProvider* m_data_provider;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifyHash(std::unique_ptr<cryptography::IHashFunction> hashFunction,
                                                       unsigned hashIndex,
                                                       IHashProvider* hashProvider,
                                                       ICapturedDataProvider* dataProvider)
    {
        return std::make_unique<StepVerifyHash>(std::move(hashFunction), hashIndex, hashProvider, dataProvider);
    }
} // namespace step
