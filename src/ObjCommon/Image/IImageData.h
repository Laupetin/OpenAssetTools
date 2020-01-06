#pragma once
#include "ImageFormat.h"
#include <cstdint>

class IImageData
{
public:
    virtual ~IImageData() = default;

    virtual unsigned GetWidth() = 0;
    virtual unsigned GetHeight() = 0;
    virtual unsigned GetDepth() = 0;

    virtual ImageFormat* GetFormat() = 0;

    virtual size_t GetSizeOfMipLevel(unsigned mipLevel) = 0;
    virtual uint8_t* GetDataForMipLevel(unsigned mipLevel) = 0;

    virtual bool HasMipMaps() = 0;
    virtual int GetMipMapCount() = 0;
    virtual void GenerateMipMaps() = 0;
    virtual void DiscardMipMaps() = 0;
};
