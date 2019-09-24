#pragma once
#include <cstdint>

class ISignatureDataProvider
{
public:
    virtual void GetSignatureData(const uint8_t** pSignatureData, size_t* pSize) = 0;
};
