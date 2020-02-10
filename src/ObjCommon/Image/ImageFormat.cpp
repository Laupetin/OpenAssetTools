#include "ImageFormat.h"

ImageFormatUnsigned::ImageFormatUnsigned(const unsigned bitPerPixel, const unsigned rOffset, const unsigned rSize,
                                         const unsigned gOffset, const unsigned gSize, const unsigned bOffset,
                                         const unsigned bSize, const unsigned aOffset, const unsigned aSize,
                                         DXGI_FORMAT dxgiFormat)
{
    m_bit_per_pixel = bitPerPixel;
    m_dxgi_format = dxgiFormat;
}

ImageFormatType ImageFormatUnsigned::GetType() const
{
    return ImageFormatType::UNSIGNED;
}

DXGI_FORMAT ImageFormatUnsigned::GetDXGIFormat() const
{
    return m_dxgi_format;
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

ImageFormatBlockCompressed::ImageFormatBlockCompressed(const unsigned blockSize, const unsigned bitsPerBlock,
                                                       DXGI_FORMAT dxgiFormat)
{
    m_block_size = blockSize;
    m_bits_per_block = bitsPerBlock;
    m_dxgi_format = dxgiFormat;
}

ImageFormatType ImageFormatBlockCompressed::GetType() const
{
    return ImageFormatType::BLOCK_COMPRESSED;
}

DXGI_FORMAT ImageFormatBlockCompressed::GetDXGIFormat() const
{
    return m_dxgi_format;
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

const ImageFormatUnsigned ImageFormat::FORMAT_R8G8B8(24, 0, 8, 8, 8, 16, 8, 0, 0, DXGI_FORMAT_UNKNOWN);
const ImageFormatUnsigned ImageFormat::FORMAT_R8G8B8A8(32, 0, 8, 8, 8, 16, 8, 24, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
const ImageFormatUnsigned ImageFormat::FORMAT_A8(8, 0, 0, 0, 0, 0, 0, 0, 8, DXGI_FORMAT_A8_UNORM);
const ImageFormatUnsigned ImageFormat::FORMAT_R16G16B16A16_FLOAT(128, 0, 0, 0, 0, 0, 0, 0, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC1(4, 64, DXGI_FORMAT_BC1_UNORM);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC2(4, 128, DXGI_FORMAT_BC2_UNORM);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC3(4, 128, DXGI_FORMAT_BC3_UNORM);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC4(4, 64, DXGI_FORMAT_BC4_UNORM);
const ImageFormatBlockCompressed ImageFormat::FORMAT_BC5(4, 128, DXGI_FORMAT_BC5_UNORM);
