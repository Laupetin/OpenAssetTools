#include "ImageFormat.h"

#include <type_traits>

namespace image
{
    ImageFormat::ImageFormat(const ImageFormatId id, std::string name, const oat::D3DFORMAT d3dFormat, const oat::DXGI_FORMAT dxgiFormat)
        : m_id(id),
          m_name(std::move(name)),
          m_d3d_format(d3dFormat),
          m_dxgi_format(dxgiFormat)
    {
    }

    const ImageFormat* ImageFormat::GetImageFormatById(const ImageFormatId id)
    {
        return format::ALL[std::to_underlying(id)];
    }

    ImageFormatId ImageFormat::GetId() const
    {
        return m_id;
    }

    const std::string& ImageFormat::GetName() const
    {
        return m_name;
    }

    oat::D3DFORMAT ImageFormat::GetD3DFormat() const
    {
        return m_d3d_format;
    }

    oat::DXGI_FORMAT ImageFormat::GetDxgiFormat() const
    {
        return m_dxgi_format;
    }

    ImageFormatUnsigned::ImageFormatUnsigned(const ImageFormatId id,
                                             std::string name,
                                             const oat::D3DFORMAT d3dFormat,
                                             const oat::DXGI_FORMAT dxgiFormat,
                                             const unsigned bitsPerPixel,
                                             const unsigned rOffset,
                                             const unsigned rSize,
                                             const unsigned gOffset,
                                             const unsigned gSize,
                                             const unsigned bOffset,
                                             const unsigned bSize,
                                             const unsigned aOffset,
                                             const unsigned aSize)
        : ImageFormat(id, std::move(name), d3dFormat, dxgiFormat),
          m_bits_per_pixel(bitsPerPixel),
          m_r_offset(rOffset),
          m_r_size(rSize),
          m_g_offset(gOffset),
          m_g_size(gSize),
          m_b_offset(bOffset),
          m_b_size(bSize),
          m_a_offset(aOffset),
          m_a_size(aSize)
    {
    }

    ImageFormatType ImageFormatUnsigned::GetType() const
    {
        return ImageFormatType::UNSIGNED;
    }

    size_t ImageFormatUnsigned::GetPitch(const unsigned mipLevel, const unsigned width) const
    {
        unsigned mipWidth = width >> mipLevel;
        if (mipWidth == 0)
            mipWidth = 1;

        return mipWidth * m_bits_per_pixel / 8;
    }

    size_t ImageFormatUnsigned::GetSizeOfMipLevel(const unsigned mipLevel, const unsigned width, const unsigned height, const unsigned depth) const
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

        return mipWidth * mipHeight * mipDepth * m_bits_per_pixel / 8;
    }

    bool ImageFormatUnsigned::HasR() const
    {
        return m_r_size > 0;
    }

    bool ImageFormatUnsigned::HasG() const
    {
        return m_g_size > 0;
    }

    bool ImageFormatUnsigned::HasB() const
    {
        return m_b_size > 0;
    }

    bool ImageFormatUnsigned::HasA() const
    {
        return m_a_size > 0;
    }

    ImageFormatBlockCompressed::ImageFormatBlockCompressed(const ImageFormatId id,
                                                           std::string name,
                                                           const oat::D3DFORMAT d3dFormat,
                                                           const oat::DXGI_FORMAT dxgiFormat,
                                                           const unsigned blockSize,
                                                           const unsigned bitsPerBlock,
                                                           const bool hasR,
                                                           const bool hasG,
                                                           const bool hasB,
                                                           const bool hasA)
        : ImageFormat(id, std::move(name), d3dFormat, dxgiFormat),
          m_block_size(blockSize),
          m_bits_per_block(bitsPerBlock),
          m_has_r(hasR),
          m_has_g(hasG),
          m_has_b(hasB),
          m_has_a(hasA)
    {
    }

    ImageFormatType ImageFormatBlockCompressed::GetType() const
    {
        return ImageFormatType::BLOCK_COMPRESSED;
    }

    size_t ImageFormatBlockCompressed::GetPitch(const unsigned mipLevel, const unsigned width) const
    {
        unsigned mipWidth = width >> mipLevel;

        if (mipWidth == 0)
            mipWidth = 1;

        const unsigned blockCount = (mipWidth + m_block_size - 1) / m_block_size;

        return blockCount * m_bits_per_block / 8;
    }

    size_t ImageFormatBlockCompressed::GetSizeOfMipLevel(const unsigned mipLevel, const unsigned width, const unsigned height, const unsigned depth) const
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

        const unsigned blockCount = ((mipWidth + m_block_size - 1) / m_block_size) * ((mipHeight + m_block_size - 1) / m_block_size) * mipDepth;

        return blockCount * m_bits_per_block / 8;
    }

    bool ImageFormatBlockCompressed::HasR() const
    {
        return m_has_r;
    }

    bool ImageFormatBlockCompressed::HasG() const
    {
        return m_has_g;
    }

    bool ImageFormatBlockCompressed::HasB() const
    {
        return m_has_b;
    }

    bool ImageFormatBlockCompressed::HasA() const
    {
        return m_has_a;
    }

    namespace format
    {
        const ImageFormatUnsigned B8_G8_R8(ImageFormatId::B8_G8_R8, "B8_G8_R8", oat::D3DFMT_R8G8B8, oat::DXGI_FORMAT_UNKNOWN, 24, 16, 8, 8, 8, 0, 8, 0, 0);
        const ImageFormatUnsigned
            B8_G8_R8_X8(ImageFormatId::B8_G8_R8_X8, "B8_G8_R8_X8", oat::D3DFMT_X8R8G8B8, oat::DXGI_FORMAT_B8G8R8X8_UNORM, 32, 16, 8, 8, 8, 0, 8, 0, 0);
        const ImageFormatUnsigned
            R8_G8_B8_A8(ImageFormatId::R8_G8_B8_A8, "R8_G8_B8_A8", oat::D3DFMT_A8B8G8R8, oat::DXGI_FORMAT_R8G8B8A8_UNORM, 32, 0, 8, 8, 8, 16, 8, 24, 8);
        const ImageFormatUnsigned
            B8_G8_R8_A8(ImageFormatId::B8_G8_R8_A8, "B8_G8_R8_A8", oat::D3DFMT_A8R8G8B8, oat::DXGI_FORMAT_B8G8R8A8_UNORM, 32, 16, 8, 8, 8, 0, 8, 24, 8);
        const ImageFormatUnsigned A8(ImageFormatId::A8, "A8", oat::D3DFMT_A8, oat::DXGI_FORMAT_A8_UNORM, 8, 0, 0, 0, 0, 0, 0, 0, 8);
        const ImageFormatUnsigned R16_G16_B16_A16_FLOAT(ImageFormatId::R16_G16_B16_A16_FLOAT,
                                                        "R16_G16_B16_A16_FLOAT",
                                                        oat::D3DFMT_A16B16G16R16F,
                                                        oat::DXGI_FORMAT_R16G16B16A16_FLOAT,
                                                        128,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        0,
                                                        8);
        const ImageFormatUnsigned R8(ImageFormatId::R8, "R8", oat::D3DFMT_L8, oat::DXGI_FORMAT_R8_UNORM, 8, 0, 8, 0, 0, 0, 0, 0, 0);
        const ImageFormatUnsigned R8_A8(ImageFormatId::R8_A8, "R8_A8", oat::D3DFMT_A8L8, oat::DXGI_FORMAT_UNKNOWN, 16, 0, 8, 0, 0, 0, 0, 8, 8);
        const ImageFormatBlockCompressed BC1(ImageFormatId::BC1, "BC1", oat::D3DFMT_DXT1, oat::DXGI_FORMAT_BC1_UNORM, 4, 64, true, true, true, true);
        const ImageFormatBlockCompressed BC2(ImageFormatId::BC2, "BC2", oat::D3DFMT_DXT3, oat::DXGI_FORMAT_BC2_UNORM, 4, 128, true, true, true, true);
        const ImageFormatBlockCompressed BC3(ImageFormatId::BC3, "BC3", oat::D3DFMT_DXT5, oat::DXGI_FORMAT_BC3_UNORM, 4, 128, true, true, true, true);
        const ImageFormatBlockCompressed BC4(ImageFormatId::BC4, "BC4", oat::D3DFMT_UNKNOWN, oat::DXGI_FORMAT_BC4_UNORM, 4, 64, true, false, false, false);
        const ImageFormatBlockCompressed BC5(ImageFormatId::BC5, "BC5", oat::D3DFMT_UNKNOWN, oat::DXGI_FORMAT_BC5_UNORM, 4, 128, true, true, false, false);

        const ImageFormat* const ALL[static_cast<unsigned>(ImageFormatId::MAX)]{
            &B8_G8_R8,
            &B8_G8_R8_X8,
            &R8_G8_B8_A8,
            &B8_G8_R8_A8,
            &A8,
            &R16_G16_B16_A16_FLOAT,
            &R8,
            &R8_A8,
            &BC1,
            &BC2,
            &BC3,
            &BC4,
            &BC5,
        };
    } // namespace format
} // namespace image
