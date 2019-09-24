#include "AlgorithmRSA.h"
#include <cstring>

#include "CryptoLibrary.h"

class AlgorithmRSA::AlgorithmRSAImpl
{
    rsa_key m_key{};
    HashingAlgorithm m_hash;
    Crypto::RSAPaddingMode m_padding;

    const ltc_hash_descriptor* GetHashDescriptor() const
    {
        switch(m_hash)
        {
        case RSA_HASH_SHA256:
            return &sha256_desc;

        default:
        case RSA_HASH_SHA512:
            return &sha512_desc;
        }
    }

    int GetPaddingMode() const
    {
        switch(m_padding)
        {
        case Crypto::RSA_PADDING_PKS1:
            return LTC_PKCS_1_V1_5;

        default:
        case Crypto::RSA_PADDING_PSS:
            return LTC_PKCS_1_PSS;
        }
    }

public:
    AlgorithmRSAImpl(const HashingAlgorithm hash, const Crypto::RSAPaddingMode padding)
    {
        m_hash = hash;
        m_padding = padding;

        CryptoLibrary::Init();
    }

    ~AlgorithmRSAImpl() = default;

    AlgorithmRSAImpl(AlgorithmRSAImpl& other) = default;
    AlgorithmRSAImpl(AlgorithmRSAImpl&& other) = delete;

    AlgorithmRSAImpl& operator=(AlgorithmRSAImpl const& other) = default;
    AlgorithmRSAImpl& operator=(AlgorithmRSAImpl&& other) = delete;

    bool SetKey(const uint8_t* keyData, const size_t keySize)
    {
        return rsa_import(keyData, keySize, &m_key) == CRYPT_OK;
    }
    
    bool Verify(const uint8_t* signedData, const size_t signedDataSize, const uint8_t* signature, const size_t signatureSize)
    {
        const ltc_hash_descriptor* hashDesc = GetHashDescriptor();
        const int hashId = register_hash(hashDesc);
        const int padding = GetPaddingMode();

        int result;
        rsa_verify_hash_ex(signature, signatureSize, signedData, signedDataSize, padding, hashId, 8, &result, &m_key);

        return result == 1;
    }
};

AlgorithmRSA::AlgorithmRSA(const HashingAlgorithm hash, const Crypto::RSAPaddingMode padding)
{
    m_impl = new AlgorithmRSAImpl(hash, padding);
}

AlgorithmRSA::~AlgorithmRSA()
{
    delete m_impl;
    m_impl = nullptr;
}

AlgorithmRSA::AlgorithmRSA(AlgorithmRSA& other)
{
    m_impl = new AlgorithmRSAImpl(*other.m_impl);
}

AlgorithmRSA::AlgorithmRSA(AlgorithmRSA&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;
}

AlgorithmRSA& AlgorithmRSA::operator=(AlgorithmRSA const& other)
{
    m_impl = new AlgorithmRSAImpl(*other.m_impl);

    return *this;
}

AlgorithmRSA& AlgorithmRSA::operator=(AlgorithmRSA&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;

    return *this;
}

bool AlgorithmRSA::SetKey(const uint8_t* keyData, size_t keySize)
{
    return m_impl->SetKey(keyData, keySize);
}

bool AlgorithmRSA::Verify(const uint8_t* signedData, const size_t signedDataSize, const uint8_t* signature, const size_t signatureSize)
{
    return m_impl->Verify(signedData, signedDataSize, signature, signatureSize);
}