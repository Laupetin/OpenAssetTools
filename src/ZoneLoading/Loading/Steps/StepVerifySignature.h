#pragma once

#include "Crypto.h"
#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"
#include "Utils/ICapturedDataProvider.h"

class StepVerifySignature final : public ILoadingStep
{
    std::unique_ptr<IPublicKeyAlgorithm> m_algorithm;
    ISignatureProvider* m_signature_provider;
    ICapturedDataProvider* m_signature_data_provider;

public:
    StepVerifySignature(std::unique_ptr<IPublicKeyAlgorithm> signatureAlgorithm,
                        ISignatureProvider* signatureProvider,
                        ICapturedDataProvider* signatureDataProvider);
    ~StepVerifySignature() override = default;
    StepVerifySignature(const StepVerifySignature& other) = delete;
    StepVerifySignature(StepVerifySignature&& other) noexcept = default;
    StepVerifySignature& operator=(const StepVerifySignature& other) = delete;
    StepVerifySignature& operator=(StepVerifySignature&& other) noexcept = default;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};
