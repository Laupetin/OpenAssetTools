#pragma once

#include "Loading/ILoadingStep.h"
#include "Crypto.h"
#include "Loading/ISignatureProvider.h"
#include "Utils/ICapturedDataProvider.h"

class StepVerifySignature final : public ILoadingStep
{
    IPublicKeyAlgorithm* m_algorithm;
    ISignatureProvider* m_signature_provider;
    ICapturedDataProvider* m_signature_data_provider;

public:
    StepVerifySignature(IPublicKeyAlgorithm* signatureAlgorithm, ISignatureProvider* signatureProvider, ICapturedDataProvider* signatureDataProvider);
    ~StepVerifySignature();
    StepVerifySignature(const StepVerifySignature& other) = default;
    StepVerifySignature(StepVerifySignature&& other) noexcept = default;
    StepVerifySignature& operator=(const StepVerifySignature& other) = default;
    StepVerifySignature& operator=(StepVerifySignature&& other) noexcept = default;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};