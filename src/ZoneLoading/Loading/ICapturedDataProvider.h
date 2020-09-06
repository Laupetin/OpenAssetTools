#pragma once
#include <cstdint>

class ICapturedDataProvider
{
public:
    virtual void GetCapturedData(const uint8_t** pCapturedData, size_t* pSize) = 0;
};
