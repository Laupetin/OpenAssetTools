#pragma once

#include <cstddef>

class IHashFunction
{
public:
    virtual ~IHashFunction() = default;

    virtual size_t GetHashSize() = 0;
    virtual void Init() = 0;
    virtual void Process(const void* input, size_t inputSize) = 0;
    virtual void Finish(void* hashBuffer) = 0;
};
