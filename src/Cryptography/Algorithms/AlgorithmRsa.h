#pragma once

#include "IPublicKeyAlgorithm.h"

#include <cstdint>
#include <memory>

namespace cryptography
{
    enum class RsaPaddingMode : std::uint8_t
    {
        RSA_PADDING_PKS1,
        RSA_PADDING_PSS,
    };

    std::unique_ptr<IPublicKeyAlgorithm> CreateRsa(HashingAlgorithm hashingAlgorithm, RsaPaddingMode paddingMode);

    /**
     * \brief Decrypts data that was encrypted with the public key counterpart of the specified private key using PKCS#1 v1.5 padding.
     * \param privateKey The DER encoded private key.
     * \param privateKeySize The size of the private key in bytes.
     * \param encrypted The encrypted data.
     * \param encryptedSize The size of the encrypted data in bytes.
     * \param output The buffer to write the decrypted data to.
     * \param outputSize The size of the output buffer. Contains the size of the decrypted data after successful decryption.
     * \return \c true if the decryption was successful otherwise \c false
     */
    bool RsaDecryptPkcs1(const uint8_t* privateKey, size_t privateKeySize, const uint8_t* encrypted, size_t encryptedSize, uint8_t* output, size_t& outputSize);
} // namespace cryptography
