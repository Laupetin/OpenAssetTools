#include "StepLoadSignature.h"

#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <cassert>

StepLoadSignature::StepLoadSignature(const size_t signatureSize)
{
    m_signature_size = signatureSize;
    m_signature = new uint8_t[signatureSize];
}

StepLoadSignature::~StepLoadSignature()
{
    delete[] m_signature;
    m_signature = nullptr;
}

void StepLoadSignature::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(stream != nullptr);

    if (stream->Load(m_signature, m_signature_size) != m_signature_size)
        throw UnexpectedEndOfFileException();
}

void StepLoadSignature::GetSignature(const uint8_t** pSignature, size_t* pSize)
{
    assert(pSignature != nullptr);
    assert(pSize != nullptr);

    assert(m_signature != nullptr);

    *pSignature = m_signature;
    *pSize = m_signature_size;
}
