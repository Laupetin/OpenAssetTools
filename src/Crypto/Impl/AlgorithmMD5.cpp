#include "AlgorithmMD5.h"

#include "CryptoLibrary.h"

#include <cstdint>

class AlgorithmMD5::AlgorithmMD5Impl
{
    hash_state m_state{};

public:
    AlgorithmMD5Impl()
    {
        CryptoLibrary::Init();

        Init();
    }

    void Init()
    {
        md5_init(&m_state);
    }

    void Process(const void* input, const size_t inputSize)
    {
        md5_process(&m_state, static_cast<const uint8_t*>(input), inputSize);
    }

    void Finish(void* hashBuffer)
    {
        md5_done(&m_state, static_cast<uint8_t*>(hashBuffer));
    }
};

AlgorithmMD5::AlgorithmMD5()
{
    m_impl = new AlgorithmMD5Impl();
}

AlgorithmMD5::~AlgorithmMD5()
{
    delete m_impl;
    m_impl = nullptr;
}

size_t AlgorithmMD5::GetHashSize()
{
    return HASH_SIZE;
}

void AlgorithmMD5::Init()
{
    m_impl->Init();
}

void AlgorithmMD5::Process(const void* input, const size_t inputSize)
{
    m_impl->Process(input, inputSize);
}

void AlgorithmMD5::Finish(void* hashBuffer)
{
    m_impl->Finish(hashBuffer);
}
