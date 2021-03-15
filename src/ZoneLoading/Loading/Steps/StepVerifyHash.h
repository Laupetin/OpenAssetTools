#pragma once

#include <memory>

#include "Crypto.h"
#include "Loading/ILoadingStep.h"
#include "Utils/ICapturedDataProvider.h"
#include "Loading/IHashProvider.h"

class StepVerifyHash final : public ILoadingStep
{
    std::unique_ptr<IHashFunction> m_hash_function;
    unsigned m_hash_index;
    IHashProvider* m_hash_provider;
    ICapturedDataProvider* m_data_provider;

public:
    StepVerifyHash(std::unique_ptr<IHashFunction> hashFunction, unsigned hashIndex, IHashProvider* hashProvider, ICapturedDataProvider* dataProvider);
    ~StepVerifyHash();
    StepVerifyHash(const StepVerifyHash& other) = delete;
    StepVerifyHash(StepVerifyHash&& other) noexcept = default;
    StepVerifyHash& operator=(const StepVerifyHash& other) = delete;
    StepVerifyHash& operator=(StepVerifyHash&& other) noexcept = default;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
