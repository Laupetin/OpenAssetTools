#include "AlgorithmRsa.h"

#include "Internal/CryptoLibrary.h"

using namespace cryptography;

namespace
{
    class AlgorithmRsa final : public IPublicKeyAlgorithm
    {
    public:
        AlgorithmRsa(const HashingAlgorithm hash, const RsaPaddingMode padding)
        {
            m_hash = hash;
            m_padding = padding;

            internal::CryptoLibrary::Init();
        }

        bool SetKey(const uint8_t* keyData, const size_t keySize) override
        {
            return rsa_import(keyData, static_cast<unsigned long>(keySize), &m_key) == CRYPT_OK;
        }

        bool Verify(const uint8_t* signedData, const size_t signedDataSize, const uint8_t* signature, const size_t signatureSize) override
        {
            const ltc_hash_descriptor* hashDesc = GetHashDescriptor();
            const int hashId = register_hash(hashDesc);
            const int padding = GetPaddingMode();

            int result;
            rsa_verify_hash_ex(signature,
                               static_cast<unsigned long>(signatureSize),
                               signedData,
                               static_cast<unsigned long>(signedDataSize),
                               padding,
                               hashId,
                               8,
                               &result,
                               &m_key);

            return result == 1;
        }

    private:
        [[nodiscard]] const ltc_hash_descriptor* GetHashDescriptor() const
        {
            switch (m_hash)
            {
            case HashingAlgorithm::RSA_HASH_SHA256:
                return &sha256_desc;

            default:
            case HashingAlgorithm::RSA_HASH_SHA512:
                return &sha512_desc;
            }
        }

        [[nodiscard]] int GetPaddingMode() const
        {
            switch (m_padding)
            {
            case RsaPaddingMode::RSA_PADDING_PKS1:
                return LTC_PKCS_1_V1_5;

            default:
            case RsaPaddingMode::RSA_PADDING_PSS:
                return LTC_PKCS_1_PSS;
            }
        }

        rsa_key m_key{};
        HashingAlgorithm m_hash;
        RsaPaddingMode m_padding;
    };
} // namespace

namespace cryptography
{
    std::unique_ptr<IPublicKeyAlgorithm> CreateRsa(const HashingAlgorithm hashingAlgorithm, const RsaPaddingMode paddingMode)
    {
        return std::make_unique<AlgorithmRsa>(hashingAlgorithm, paddingMode);
    }
} // namespace cryptography
