#include "StepLoadHash.h"
#include "Loading/Exception/UnexpectedEndOfFileException.h"
#include <cassert>

StepLoadHash::StepLoadHash(const size_t hashSize, const unsigned hashCount)
    : m_hash_size(hashSize),
      m_hash_count(hashCount),
      m_hashes(std::make_unique<uint8_t[]>(hashSize * hashCount))
{
}

StepLoadHash::~StepLoadHash()
= default;

void StepLoadHash::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(stream != nullptr);

    if (stream->Load(m_hashes.get(), m_hash_size * m_hash_count) != m_hash_size * m_hash_count)
        throw UnexpectedEndOfFileException();
}

void StepLoadHash::GetHash(const unsigned hashIndex, const uint8_t** pHash, size_t* pSize)
{
    assert(pHash != nullptr);
    assert(pSize != nullptr);
    assert(hashIndex >= 0 && hashIndex < m_hash_count);

    assert(m_hashes);

    *pHash = &m_hashes[m_hash_size * hashIndex];
    *pSize = m_hash_size;
}

void StepLoadHash::GetCapturedData(const uint8_t** pCapturedData, size_t* pSize)
{
    assert(pCapturedData != nullptr);
    assert(pSize != nullptr);
    
    *pCapturedData = m_hashes.get();
    *pSize = m_hash_size * m_hash_count;
}
