#include "StepVerifyHash.h"

#include <memory>
#include <cstring>

#include "Loading/Exception/InvalidHashException.h"

StepVerifyHash::StepVerifyHash(std::unique_ptr<IHashFunction> hashFunction, const unsigned hashIndex, IHashProvider* hashProvider,
    ICapturedDataProvider* dataProvider)
    : m_hash_function(std::move(hashFunction)),
      m_hash_index(hashIndex),
      m_hash_provider(hashProvider),
      m_data_provider(dataProvider)
{
}

StepVerifyHash::~StepVerifyHash()
= default;

void StepVerifyHash::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    const uint8_t* dataToHash = nullptr;
    size_t dataToHashSize = 0;
    m_data_provider->GetCapturedData(&dataToHash, &dataToHashSize);

    const uint8_t* hashData = nullptr;
    size_t hashSize = 0;
    m_hash_provider->GetHash(m_hash_index, &hashData, &hashSize);

    if (hashSize != m_hash_function->GetHashSize())
        throw InvalidHashException();

    const std::unique_ptr<uint8_t[]> hashMemory = std::make_unique<uint8_t[]>(m_hash_function->GetHashSize());
    m_hash_function->Init();
    m_hash_function->Process(dataToHash, dataToHashSize);
    m_hash_function->Finish(hashMemory.get());

    if(std::memcmp(hashData, hashMemory.get(), m_hash_function->GetHashSize()) != 0)
        throw InvalidHashException();
}
