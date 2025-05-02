#pragma once

#include "Cryptography.h"
#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"
#include "Utils/ICapturedDataProvider.h"

class StepVerifySignature final : public ILoadingStep
{
public:
    StepVerifySignature(std::unique_ptr<cryptography::IPublicKeyAlgorithm> signatureAlgorithm,
                        ISignatureProvider* signatureProvider,
                        ICapturedDataProvider* signatureDataProvider);

    void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override;

private:
    std::unique_ptr<cryptography::IPublicKeyAlgorithm> m_algorithm;
    ISignatureProvider* m_signature_provider;
    ICapturedDataProvider* m_signature_data_provider;
};
