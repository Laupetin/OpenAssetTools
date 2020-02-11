#pragma once

#include <dxgiformat.h>

enum class ImageFormatId
{
    UNKNOWN,
    R8_G8_B8,
    R8_G8_B8_A8,
    A8,
    R16_G16_B16_A16_FLOAT,
    BC1,
    BC2,
    BC3,
    BC4,
    BC5
};

enum class ImageFormatType
{
    UNKNOWN,
    UNSIGNED,
    BLOCK_COMPRESSED
};

class ImageFormatUnsigned;
class ImageFormatBlockCompressed;

class ImageFormat
{
    ImageFormatId m_id;
    DXGI_FORMAT m_dxgi_format;

protected:
    ImageFormat(ImageFormatId id, DXGI_FORMAT dxgiFormat);

public:
    virtual ~ImageFormat() = default;

    ImageFormatId GetId() const;
    DXGI_FORMAT GetDxgiFormat() const;

    virtual ImageFormatType GetType() const = 0;
    virtual size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const = 0;

    static const ImageFormatUnsigned FORMAT_R8_G8_B8;
    static const ImageFormatUnsigned FORMAT_R8_G8_B8_A8;
    static const ImageFormatUnsigned FORMAT_A8;
    static const ImageFormatUnsigned FORMAT_R16_G16_B16_A16_FLOAT; //TODO: Float not unsigned
    static const ImageFormatBlockCompressed FORMAT_BC1;
    static const ImageFormatBlockCompressed FORMAT_BC2;
    static const ImageFormatBlockCompressed FORMAT_BC3;
    static const ImageFormatBlockCompressed FORMAT_BC4;
    static const ImageFormatBlockCompressed FORMAT_BC5;
};

class ImageFormatUnsigned final : public ImageFormat
{
    unsigned m_bit_per_pixel;

public:
    ImageFormatUnsigned(ImageFormatId id, DXGI_FORMAT dxgiFormat, unsigned bitPerPixel, unsigned rOffset,
                        unsigned rSize, unsigned gOffset, unsigned gSize, unsigned bOffset, unsigned bSize,
                        unsigned aOffset, unsigned aSize);

    ImageFormatType GetType() const override;
    size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;
};

class ImageFormatBlockCompressed final : public ImageFormat
{
    unsigned m_block_size;
    unsigned m_bits_per_block;

public:
    ImageFormatBlockCompressed(ImageFormatId id, DXGI_FORMAT dxgiFormat, unsigned blockSize, unsigned bitsPerBlock);

    ImageFormatType GetType() const override;
    size_t GetSizeOfMipLevel(unsigned mipLevel, unsigned width, unsigned height, unsigned depth) const override;
};
