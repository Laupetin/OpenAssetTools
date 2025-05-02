#pragma once
#include <cstdint>

class ISignatureProvider
{
public:
    ISignatureProvider() = default;
    virtual ~ISignatureProvider() = default;
    ISignatureProvider(const ISignatureProvider& other) = default;
    ISignatureProvider(ISignatureProvider&& other) noexcept = default;
    ISignatureProvider& operator=(const ISignatureProvider& other) = default;
    ISignatureProvider& operator=(ISignatureProvider&& other) noexcept = default;

    virtual void GetSignature(const uint8_t** pSignature, size_t* pSize) = 0;
};
