#pragma once

#include "Loading/ILoadingStep.h"
#include "Crypto.h"
#include "Loading/ISignatureProvider.h"
#include "Loading/ISignatureDataProvider.h"

class StepVerifySignature final : public ILoadingStep
{
    IPublicKeyAlgorithm* m_algorithm;
    ISignatureProvider* m_signature_provider;
    ISignatureDataProvider* m_signature_data_provider;

public:
    StepVerifySignature(IPublicKeyAlgorithm* signatureAlgorithm, ISignatureProvider* signatureProvider, ISignatureDataProvider* signatureDataProvider);
    ~StepVerifySignature();

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
};