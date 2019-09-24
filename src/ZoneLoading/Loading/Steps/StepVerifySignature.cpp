#include "StepVerifySignature.h"
#include "Loading/Exception/InvalidSignatureException.h"
#include <cassert>

StepVerifySignature::StepVerifySignature(IPublicKeyAlgorithm* signatureAlgorithm, ISignatureProvider* signatureProvider, ISignatureDataProvider* signatureDataProvider)
{
    m_algorithm = signatureAlgorithm;
    m_signature_provider = signatureProvider;
    m_signature_data_provider = signatureDataProvider;
}

StepVerifySignature::~StepVerifySignature()
{
    delete m_algorithm;
    m_algorithm = nullptr;
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
    m_signature_data_provider->GetSignatureData(&signatureData, &signatureDataSize);

    if(!m_algorithm->Verify(signatureData, signatureDataSize, signature, signatureSize))
    {
        throw InvalidSignatureException();
    }
}