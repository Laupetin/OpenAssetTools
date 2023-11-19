#pragma once
#include "Crypto.h"
#include "IPublicKeyAlgorithm.h"

#include <cstdint>

class AlgorithmRSA final : public IPublicKeyAlgorithm
{
    class AlgorithmRSAImpl;
    AlgorithmRSAImpl* m_impl;

public:
    AlgorithmRSA(HashingAlgorithm hash, Crypto::RSAPaddingMode padding);
    ~AlgorithmRSA() override;

    AlgorithmRSA(AlgorithmRSA& other);
    AlgorithmRSA(AlgorithmRSA&& other) noexcept;

    AlgorithmRSA& operator=(AlgorithmRSA const& other);
    AlgorithmRSA& operator=(AlgorithmRSA&& other) noexcept;

    bool SetKey(const uint8_t* keyData, size_t keySize) override;

    bool Verify(const uint8_t* signedData, size_t signedDataSize, const uint8_t* signature, size_t signatureSize) override;
};
