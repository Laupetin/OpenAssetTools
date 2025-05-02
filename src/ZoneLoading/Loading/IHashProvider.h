#pragma once

#include <cstdint>

class IHashProvider
{
public:
    IHashProvider() = default;
    virtual ~IHashProvider() = default;
    IHashProvider(const IHashProvider& other) = default;
    IHashProvider(IHashProvider&& other) noexcept = default;
    IHashProvider& operator=(const IHashProvider& other) = default;
    IHashProvider& operator=(IHashProvider&& other) noexcept = default;

    virtual void GetHash(unsigned hashIndex, const uint8_t** pHash, size_t* pSize) = 0;
};
