#pragma once
#include <cstdint>

class IHashProvider
{
public:
    virtual void GetHash(unsigned hashIndex, const uint8_t** pHash, size_t* pSize) = 0;
};
