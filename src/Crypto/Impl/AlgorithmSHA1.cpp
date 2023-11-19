#include "AlgorithmSHA1.h"

#include "CryptoLibrary.h"

#include <cstdint>

class AlgorithmSHA1::AlgorithmSHA1Impl
{
    hash_state m_state{};

public:
    AlgorithmSHA1Impl()
    {
        CryptoLibrary::Init();

        Init();
    }

    void Init()
    {
        sha1_init(&m_state);
    }

    void Process(const void* input, const size_t inputSize)
    {
        sha1_process(&m_state, static_cast<const uint8_t*>(input), inputSize);
    }

    void Finish(void* hashBuffer)
    {
        sha1_done(&m_state, static_cast<uint8_t*>(hashBuffer));
    }
};

AlgorithmSHA1::AlgorithmSHA1()
{
    m_impl = new AlgorithmSHA1Impl();
}

AlgorithmSHA1::~AlgorithmSHA1()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t AlgorithmSHA1::GetHashSize()
{
    return HASH_SIZE;
}

void AlgorithmSHA1::Init()
{
    m_impl->Init();
}

void AlgorithmSHA1::Process(const void* input, const size_t inputSize)
{
    m_impl->Process(input, inputSize);
}

void AlgorithmSHA1::Finish(void* hashBuffer)
{
    m_impl->Finish(hashBuffer);
}
