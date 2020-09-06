#include "AlgorithmSHA256.h"

#include "CryptoLibrary.h"
#include <cstdint>

class AlgorithmSHA256::Impl
{
    hash_state m_state{};

public:
    Impl()
    {
        CryptoLibrary::Init();

        Init();
    }

    void Init()
    {
        sha256_init(&m_state);
    }

    void Process(const void* input, const size_t inputSize)
    {
        sha256_process(&m_state, static_cast<const uint8_t*>(input), inputSize);
    }

    void Finish(void* hashBuffer)
    {
        sha256_done(&m_state, static_cast<uint8_t*>(hashBuffer));
    }
};

AlgorithmSHA256::AlgorithmSHA256()
{
    m_impl = new Impl();
}

AlgorithmSHA256::~AlgorithmSHA256()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t AlgorithmSHA256::GetHashSize()
{
    return HASH_SIZE;
}

void AlgorithmSHA256::Init()
{
    m_impl->Init();
}

void AlgorithmSHA256::Process(const void* input, const size_t inputSize)
{
    m_impl->Process(input, inputSize);
}

void AlgorithmSHA256::Finish(void* hashBuffer)
{
    m_impl->Finish(hashBuffer);
}