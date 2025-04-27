#include "StepVerifySignature.h"

#include "Loading/Exception/InvalidSignatureException.h"

#include <cassert>

StepVerifySignature::StepVerifySignature(std::unique_ptr<cryptography::IPublicKeyAlgorithm> signatureAlgorithm,
                                         ISignatureProvider* signatureProvider,
                                         ICapturedDataProvider* signatureDataProvider)
    : m_algorithm(std::move(signatureAlgorithm)),
      m_signature_provider(signatureProvider),
      m_signature_data_provider(signatureDataProvider)
{
}

void StepVerifySignature::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(m_algorithm != nullptr);
    assert(m_signature_provider != nullptr);
    assert(m_signature_data_provider != nullptr);

    const uint8_t* signature;
    size_t signatureSize;
    m_signature_provider->GetSignature(&signature, &signatureSize);

    const uint8_t* signatureData;
    size_t signatureDataSize;
    m_signature_data_provider->GetCapturedData(&signatureData, &signatureDataSize);

    if (!m_algorithm->Verify(signatureData, signatureDataSize, signature, signatureSize))
    {
        throw InvalidSignatureException();
    }
}
