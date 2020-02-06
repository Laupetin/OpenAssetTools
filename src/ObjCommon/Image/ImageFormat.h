#pragma once

#include <dxgiformat.h>

enum class ImageFormatType
{
    UNKNOWN,
    UNSIGNED,
    BLOCK_COMPRESSED
};

class IImageFormat
{
public:
    virtual ~IImageFormat() = default;

    virtual ImageFormatType GetType() const = 0;
    virtual DXGI_FORMAT GetDXGIFormat() const = 0;
    virtual size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const = 0;
};

class ImageFormatUnsigned final : public IImageFormat
{
    unsigned m_bit_per_pixel;
    DXGI_FORMAT m_dxgi_format;

public:
    ImageFormatUnsigned(unsigned bitPerPixel, unsigned rOffset, unsigned rSize, unsigned gOffset, unsigned gSize,
                        unsigned bOffset, unsigned bSize, unsigned aOffset, unsigned aSize, DXGI_FORMAT dxgiFormat);

    ImageFormatType GetType() const override;
    DXGI_FORMAT GetDXGIFormat() const override;
    size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;
};

class ImageFormatBlockCompressed final : public IImageFormat
{
    unsigned m_block_size;
    unsigned m_bits_per_block;
    DXGI_FORMAT m_dxgi_format;

public:
    ImageFormatBlockCompressed(unsigned blockSize, unsigned bitsPerBlock, DXGI_FORMAT dxgiFormat);

    ImageFormatType GetType() const override;
    DXGI_FORMAT GetDXGIFormat() const override;
    size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;
};

class ImageFormat
{
public:
    ImageFormat() = delete;

    static const ImageFormatUnsigned FORMAT_R8G8B8;
    static const ImageFormatBlockCompressed FORMAT_BC1;
    static const ImageFormatBlockCompressed FORMAT_BC2;
    static const ImageFormatBlockCompressed FORMAT_BC3;
};
