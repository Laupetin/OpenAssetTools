#pragma once
#include "IHashFunction.h"

class AlgorithmSHA256 : public IHashFunction
{
    class Impl;
    Impl* m_impl;

public:
    static const int HASH_SIZE = 32;

    AlgorithmSHA256();
    ~AlgorithmSHA256() override;

    size_t GetHashSize() override;

    void Init() override;
    void Process(const void* input, size_t inputSize) override;
    void Finish(void* hashBuffer) override;
};
