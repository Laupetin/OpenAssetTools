#pragma once
#include "IHashFunction.h"

class AlgorithmMD5 : public IHashFunction
{
    class AlgorithmMD5Impl;
    AlgorithmMD5Impl* m_impl;

public:
    static const int HASH_SIZE = 20;

    AlgorithmMD5();
    ~AlgorithmMD5() override;

    size_t GetHashSize() override;

    void Init() override;
    void Process(const void* input, size_t inputSize) override;
    void Finish(void* hashBuffer) override;
};
