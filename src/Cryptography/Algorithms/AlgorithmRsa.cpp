#include "AlgorithmRsa.h"

#include "Internal/CryptoLibrary.h"
#include "tomcrypt_private.h"

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
            const ltc_rsa_parameters rsaParams{
                .saltlen = 8,
                .hash_idx = hashId,
                .mgf1_hash_idx = hashId,
            };
            ltc_rsa_op_parameters params{
                .params = rsaParams,
                .padding = padding,
                .wprng = -1,
                .prng = nullptr,
                .u = {},
            };

            rsa_verify_hash_v2(
                signature, static_cast<unsigned long>(signatureSize), signedData, static_cast<unsigned long>(signedDataSize), &params, &result, &m_key);

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
    bool RsaDecryptPkcs1(
        const uint8_t* privateKey, const size_t privateKeySize, const uint8_t* encrypted, const size_t encryptedSize, uint8_t* output, size_t& outputSize)
    {
        internal::CryptoLibrary::Init();

        rsa_key key{};
        if (rsa_import(privateKey, static_cast<unsigned long>(privateKeySize), &key) != CRYPT_OK)
            return false;

        ltc_rsa_op_parameters params{
            .params =
                ltc_rsa_parameters{
                                   .saltlen = 0,
                                   .hash_idx = -1,
                                   .mgf1_hash_idx = -1,
                                   },
            .padding = LTC_PKCS_1_V1_5,
            .wprng = -1,
            .prng = nullptr,
            .u = {},
        };

        auto decryptedSize = static_cast<unsigned long>(outputSize);
        int stat = 0;
        const auto result = rsa_decrypt_key_v2(encrypted, static_cast<unsigned long>(encryptedSize), output, &decryptedSize, &params, &stat, &key);
        rsa_free(&key);

        if (result != CRYPT_OK || stat != 1)
            return false;

        outputSize = static_cast<size_t>(decryptedSize);
        return true;
    }

    std::unique_ptr<IPublicKeyAlgorithm> CreateRsa(const HashingAlgorithm hashingAlgorithm, const RsaPaddingMode paddingMode)
    {
        return std::make_unique<AlgorithmRsa>(hashingAlgorithm, paddingMode);
    }
} // namespace cryptography
