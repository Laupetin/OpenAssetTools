#include "StepVerifySignature.h"

#include "Loading/Exception/InvalidSignatureException.h"

#include <cassert>

namespace
{
    class StepVerifySignature final : public ILoadingStep
    {
    public:
        StepVerifySignature(std::unique_ptr<cryptography::IPublicKeyAlgorithm> signatureAlgorithm,
                            ISignatureProvider* signatureProvider,
                            ICapturedDataProvider* signatureDataProvider)
            : m_algorithm(std::move(signatureAlgorithm)),
              m_signature_provider(signatureProvider),
              m_signature_data_provider(signatureDataProvider)
        {
        }

        void PerformStep(ZoneLoader& zoneLoader, ILoadingStream& stream) override
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

    private:
        std::unique_ptr<cryptography::IPublicKeyAlgorithm> m_algorithm;
        ISignatureProvider* m_signature_provider;
        ICapturedDataProvider* m_signature_data_provider;
    };
} // namespace

namespace step
{
    std::unique_ptr<ILoadingStep> CreateStepVerifySignature(std::unique_ptr<cryptography::IPublicKeyAlgorithm> signatureAlgorithm,
                                                            ISignatureProvider* signatureProvider,
                                                            ICapturedDataProvider* signatureDataProvider)
    {
        return std::make_unique<StepVerifySignature>(std::move(signatureAlgorithm), signatureProvider, signatureDataProvider);
    }
} // namespace step
