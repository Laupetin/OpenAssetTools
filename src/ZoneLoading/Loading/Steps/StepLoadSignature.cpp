#include "StepLoadSignature.h"

#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <cassert>

StepLoadSignature::StepLoadSignature(const size_t signatureSize)
    : m_signature(std::make_unique<uint8_t[]>(signatureSize)),
      m_signature_size(signatureSize)
{
}

void StepLoadSignature::PerformStep(ZoneLoader* zoneLoader, ILoadingStream* stream)
{
    assert(stream != nullptr);

    if (stream->Load(m_signature.get(), m_signature_size) != m_signature_size)
        throw UnexpectedEndOfFileException();
}

void StepLoadSignature::GetSignature(const uint8_t** pSignature, size_t* pSize)
{
    assert(pSignature != nullptr);
    assert(pSize != nullptr);

    assert(m_signature != nullptr);

    *pSignature = m_signature.get();
    *pSize = m_signature_size;
}
