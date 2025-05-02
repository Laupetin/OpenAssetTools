#pragma once

#include "Loading/IHashProvider.h"
#include "Loading/ILoadingStep.h"
#include "Utils/ICapturedDataProvider.h"

#include <cstdint>
#include <memory>

class StepLoadHash final : public ILoadingStep, public IHashProvider, public ICapturedDataProvider
{
public:
    StepLoadHash(size_t hashSize, unsigned hashCount);
    ~StepLoadHash() override;

    StepLoadHash(const StepLoadHash& other) = delete;
    StepLoadHash(StepLoadHash&& other) noexcept = default;
    StepLoadHash& operator=(const StepLoadHash& other) = delete;
    StepLoadHash& operator=(StepLoadHash&& other) noexcept = delete;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
    void GetHash(unsigned hashIndex, const uint8_t** pHash, size_t* pSize) override;
    void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) override;

private:
    const size_t m_hash_size;
    const unsigned m_hash_count;
    std::unique_ptr<uint8_t[]> m_hashes;
};
