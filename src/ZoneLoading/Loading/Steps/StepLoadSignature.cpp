#include "StepLoadSignature.h"

#include "Loading/Exception/UnexpectedEndOfFileException.h"

#include <cassert>

namespace
{
    class StepLoadSignature final : public step::IStepLoadSignature
    {
    public:
        explicit StepLoadSignature(const size_t signatureSize)
            : m_signature(std::make_unique<uint8_t[]>(signatureSize)),
              m_signature_size(signatureSize)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
        {
            if (stream.Load(m_signature.get(), m_signature_size) != m_signature_size)
                throw UnexpectedEndOfFileException();
        }

        void GetSignature(const uint8_t** pSignature, size_t* pSize) override
        {
            assert(pSignature != nullptr);
            assert(pSize != nullptr);

            assert(m_signature != nullptr);

            *pSignature = m_signature.get();
            *pSize = m_signature_size;
        }

    private:
        std::unique_ptr<uint8_t[]> m_signature;
        size_t m_signature_size;
    };
} // namespace

namespace step
{
    std::unique_ptr<IStepLoadSignature> CreateStepLoadSignature(const size_t signatureSize)
    {
        return std::make_unique<StepLoadSignature>(signatureSize);
    }
} // namespace step
