#include "ImageFormat.h"


ImageFormat::ImageFormat(const ImageFormatId id, const DXGI_FORMAT dxgiFormat)
{
    m_id = id;
    m_dxgi_format = dxgiFormat;
}

ImageFormatId ImageFormat::GetId() const
{
    return m_id;
}

DXGI_FORMAT ImageFormat::GetDxgiFormat() const
{
    return m_dxgi_format;
}

ImageFormatUnsigned::ImageFormatUnsigned(const ImageFormatId id, const DXGI_FORMAT dxgiFormat,
                                         const unsigned bitPerPixel, const unsigned rOffset, const unsigned rSize,
                                         const unsigned gOffset, const unsigned gSize, const unsigned bOffset,
                                         const unsigned bSize, const unsigned aOffset, const unsigned aSize)
    : ImageFormat(id, dxgiFormat)
{
    m_bit_per_pixel = bitPerPixel;
}

ImageFormatType ImageFormatUnsigned::GetType() const
{
    return ImageFormatType::UNSIGNED;
}

size_t ImageFormatUnsigned::GetSizeOfMipLevel(const unsigned mipLevel, const unsigned width, const unsigned height,
                                              const unsigned depth) const
{
    unsigned mipWidth = width >> mipLevel;
    unsigned mipHeight = height >> mipLevel;
    unsigned mipDepth = depth >> mipLevel;

    if (mipWidth == 0)
        mipWidth = 1;
    if (mipHeight == 0)
        mipHeight = 1;
    if (mipDepth == 0)
        mipDepth = 1;

    return mipWidth * mipHeight * mipDepth * m_bit_per_pixel / 8;
}

ImageFormatBlockCompressed::ImageFormatBlockCompressed(const ImageFormatId id, const DXGI_FORMAT dxgiFormat,
                                                       const unsigned blockSize, const unsigned bitsPerBlock)
    : ImageFormat(id, dxgiFormat)
{
    m_block_size = blockSize;
    m_bits_per_block = bitsPerBlock;
}

ImageFormatType ImageFormatBlockCompressed::GetType() const
{
    return ImageFormatType::BLOCK_COMPRESSED;
}

size_t ImageFormatBlockCompressed::GetSizeOfMipLevel(const unsigned mipLevel, const unsigned width,
                                                     const unsigned height, const unsigned depth) const
{
    unsigned mipWidth = width >> mipLevel;
    unsigned mipHeight = height >> mipLevel;
    unsigned mipDepth = depth >> mipLevel;

    if (mipWidth == 0)
        mipWidth = 1;
    if (mipHeight == 0)
        mipHeight = 1;
    if (mipDepth == 0)
        mipDepth = 1;

    const unsigned blockCount = ((mipWidth + m_block_size - 1) / m_block_size)
        * ((mipHeight + m_block_size - 1) / m_block_size)
        * mipDepth;

    return blockCount * m_bits_per_block / 8;
}

const ImageFormatUnsigned ImageFormat::FORMAT_R8_G8_B8(ImageFormatId::R8_G8_B8, DXGI_FORMAT_UNKNOWN,
                                                       24, 0, 8, 8, 8, 16, 8, 0, 0);
const ImageFormatUnsigned ImageFormat::FORMAT_R8_G8_B8_A8(ImageFormatId::R8_G8_B8_A8, DXGI_FORMAT_R8G8B8A8_UNORM,
                                                          32, 0, 8, 8, 8, 16, 8, 24, 8);
const ImageFormatUnsigned ImageFormat::FORMAT_A8(ImageFormatId::A8, DXGI_FORMAT_A8_UNORM,
                                                 8, 0, 0, 0, 0, 0, 0, 0, 8);
const ImageFormatUnsigned ImageFormat::FORMAT_R16_G16_B16_A16_FLOAT(ImageFormatId::R16_G16_B16_A16_FLOAT,
                                                                    DXGI_FORMAT_R16G16B16A16_FLOAT,
                                                                    128, 0, 0, 0, 0, 0, 0, 0, 8);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC1(ImageFormatId::BC1, DXGI_FORMAT_BC1_UNORM, 4, 64);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC2(ImageFormatId::BC2, DXGI_FORMAT_BC2_UNORM, 4, 128);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC3(ImageFormatId::BC3, DXGI_FORMAT_BC3_UNORM, 4, 128);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC4(ImageFormatId::BC4, DXGI_FORMAT_BC4_UNORM, 4, 64);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC5(ImageFormatId::BC5, DXGI_FORMAT_BC5_UNORM, 4, 128);
