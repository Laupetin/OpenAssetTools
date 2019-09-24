#pragma once
#include <cstdint>

class ISignatureProvider
{
public:
    virtual void GetSignature(const uint8_t** pSignature, size_t* pSize) = 0;
};
