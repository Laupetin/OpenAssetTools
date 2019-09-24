#pragma once
#include "IHashFunction.h"

class AlgorithmSHA1 : public IHashFunction
{
    class AlgorithmSHA1Impl;
    AlgorithmSHA1Impl* m_impl;

public:
    static const int HASH_SIZE = 20;

    AlgorithmSHA1();
    ~AlgorithmSHA1() override;

    size_t GetHashSize() override;

    void Init() override;
    void Process(const void* input, size_t inputSize) override;
    void Finish(void* hashBuffer) override;
};
