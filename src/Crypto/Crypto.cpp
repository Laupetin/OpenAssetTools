#include "Crypto.h"

#include "Impl/AlgorithmRSA.h"
#include "Impl/AlgorithmSHA1.h"
#include "Impl/AlgorithmSHA256.h"
#include "Impl/AlgorithmSalsa20.h"

std::unique_ptr<IHashFunction> Crypto::CreateSHA1()
{
    return std::make_unique<AlgorithmSHA1>();
}

std::unique_ptr<IHashFunction> Crypto::CreateSHA256()
{
    return std::make_unique<AlgorithmSHA256>();
}

std::unique_ptr<IStreamCipher> Crypto::CreateSalsa20(const uint8_t* keyBytes, const size_t keySize)
{
    return std::make_unique<AlgorithmSalsa20>(keyBytes, keySize);
}

std::unique_ptr<IPublicKeyAlgorithm> Crypto::CreateRSA(const IPublicKeyAlgorithm::HashingAlgorithm hashingAlgorithm, const RSAPaddingMode paddingMode)
{
    return std::make_unique<AlgorithmRSA>(hashingAlgorithm, paddingMode);
}
