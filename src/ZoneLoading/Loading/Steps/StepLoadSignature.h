#pragma once

#include "Loading/ILoadingStep.h"
#include "Loading/ISignatureProvider.h"

#include <memory>

class StepLoadSignature final : public ILoadingStep, public ISignatureProvider
{
public:
    explicit StepLoadSignature(size_t signatureSize);

    void PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream) override;
    void GetSignature(const uint8_t** pSignature, size_t* pSize) override;

private:
    std::unique_ptr<uint8_t[]> m_signature;
    size_t m_signature_size;
};
