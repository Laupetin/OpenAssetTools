#pragma once

#include "Cryptography.h"
#include "Loading/IHashProvider.h"
#include "Loading/ILoadingStep.h"
#include "Utils/ICapturedDataProvider.h"

#include <memory>

class StepVerifyHash final : public ILoadingStep
{
    std::unique_ptr<cryptography::IHashFunction> m_hash_function;
    unsigned m_hash_index;
    IHashProvider* m_hash_provider;
    ICapturedDataProvider* m_data_provider;

public:
    StepVerifyHash(std::unique_ptr<cryptography::IHashFunction> hashFunction,
                   unsigned hashIndex,
                   IHashProvider* hashProvider,
                   ICapturedDataProvider* dataProvider);
    ~StepVerifyHash();
    StepVerifyHash(const StepVerifyHash& other) = delete;
    StepVerifyHash(StepVerifyHash&& other) noexcept = default;
    StepVerifyHash& operator=(const StepVerifyHash& other) = delete;
    StepVerifyHash& operator=(StepVerifyHash&& other) noexcept = default;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
