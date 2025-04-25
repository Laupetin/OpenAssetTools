#pragma once

#include <cstdlib>

namespace cryptography
{
    class IHashFunction
    {
    public:
        IHashFunction() = default;
        virtual ~IHashFunction() = default;
        IHashFunction(const IHashFunction& other) = default;
        IHashFunction(IHashFunction&& other) noexcept = default;
        IHashFunction& operator=(const IHashFunction& other) = default;
        IHashFunction& operator=(IHashFunction&& other) noexcept = default;

        virtual size_t GetHashSize() = 0;
        virtual void Init() = 0;
        virtual void Process(const void* input, size_t inputSize) = 0;
        virtual void Finish(void* hashBuffer) = 0;
    };
} // namespace cryptography
