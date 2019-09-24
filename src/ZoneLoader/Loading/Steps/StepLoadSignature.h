#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"

class StepLoadSignature final : public ILoadingStep, public ISignatureProvider
{
    uint8_t* m_signature;
    size_t m_signature_size;

public:
    explicit StepLoadSignature(size_t signatureSize);
    ~StepLoadSignature() override;

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
    void GetSignature(const uint8_t** pSignature, size_t* pSize) override;
};
