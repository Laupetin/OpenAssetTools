#pragma once

#include <dxgiformat.h>

class IImageFormat
{
public:
    virtual DXGI_FORMAT GetDXGIFormat() = 0;
    virtual size_t GetSizeForMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth);
};

class ImageFormat
{
public:
    ImageFormat(unsigned bitPerPixel, unsigned rOffset, unsigned rSize, unsigned gOffset, unsigned gSize, unsigned bOffset, unsigned bSize, unsigned aOffset, unsigned aSize);

    DXGI_FORMAT GetDXGIFormat();
    size_t GetSizeForMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth);
};