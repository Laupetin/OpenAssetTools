#include "DdsWriter.h"

#include "Image/DdsTypes.h"
#include "Image/TextureConverter.h"

#include <cassert>

using namespace image;

namespace
{
    class DdsWriterInternal
    {
    public:
        DdsWriterInternal(std::ostream& stream, const Texture* texture)
            : m_stream(stream),
              m_texture(texture),
              m_use_dx10_extension(false)
        {
        }

        void DumpImage()
        {
            DDS_HEADER header{};
            PopulateDdsHeader(header);

            constexpr auto magic = MakeFourCc('D', 'D', 'S', ' ');

            m_stream.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
            m_stream.write(reinterpret_cast<const char*>(&header), sizeof(header));

            if (m_use_dx10_extension)
            {
                DDS_HEADER_DXT10 dxt10{};
                PopulateDxt10Header(dxt10);
                m_stream.write(reinterpret_cast<const char*>(&dxt10), sizeof(dxt10));
            }

            const auto mipCount = m_texture->HasMipMaps() ? m_texture->GetMipMapCount() : 1;
            for (auto mipLevel = 0; mipLevel < mipCount; mipLevel++)
            {
                const auto* buffer = m_texture->GetBufferForMipLevel(mipLevel);
                const auto mipLevelSize = m_texture->GetSizeOfMipLevel(mipLevel) * m_texture->GetFaceCount();
                m_stream.write(reinterpret_cast<const char*>(buffer), static_cast<std::streamsize>(mipLevelSize));
            }
        }

        static constexpr unsigned Mask1(const unsigned length)
        {
            if (length >= sizeof(unsigned) * 8)
                return UINT32_MAX;

            return UINT32_MAX >> (sizeof(unsigned) * 8 - length);
        }

        void PopulatePixelFormatBlockCompressed(DDS_PIXELFORMAT& pf, const ImageFormatBlockCompressed* format)
        {
            pf.dwSize = sizeof(DDS_PIXELFORMAT);
            pf.dwFlags = DDPF_FOURCC;
            pf.dwRGBBitCount = 0;
            pf.dwRBitMask = 0;
            pf.dwGBitMask = 0;
            pf.dwBBitMask = 0;
            pf.dwABitMask = 0;

            // Use standard pixel format for DXT1-5 for maximum compatibility and only otherwise use DX10 extension
            switch (format->GetDxgiFormat())
            {
            case oat::DXGI_FORMAT_BC1_UNORM:
                pf.dwFourCC = MakeFourCc('D', 'X', 'T', '1');
                break;
            case oat::DXGI_FORMAT_BC2_UNORM:
                pf.dwFourCC = MakeFourCc('D', 'X', 'T', '3');
                break;
            case oat::DXGI_FORMAT_BC3_UNORM:
                pf.dwFourCC = MakeFourCc('D', 'X', 'T', '5');
                break;
            default:
                m_use_dx10_extension = true;
                pf.dwFourCC = MakeFourCc('D', 'X', '1', '0');
                break;
            }
        }

        static void PopulatePixelFormatUnsigned(DDS_PIXELFORMAT& pf, const ImageFormatUnsigned* format)
        {
            pf.dwSize = sizeof(DDS_PIXELFORMAT);
            pf.dwFourCC = 0;
            pf.dwRGBBitCount = format->m_bits_per_pixel;
            pf.dwRBitMask = format->HasR() ? Mask1(format->m_r_size) << format->m_r_offset : 0;
            pf.dwGBitMask = format->HasG() ? Mask1(format->m_g_size) << format->m_g_offset : 0;
            pf.dwBBitMask = format->HasB() ? Mask1(format->m_b_size) << format->m_b_offset : 0;
            pf.dwABitMask = format->HasA() ? Mask1(format->m_a_size) << format->m_a_offset : 0;

            pf.dwFlags = 0;
            if (format->HasA())
                pf.dwFlags |= DDPF_ALPHAPIXELS;

            if (format->HasR() && !format->HasG() && !format->HasB())
                pf.dwFlags |= DDPF_LUMINANCE;
            else
                pf.dwFlags |= DDPF_RGB;
        }

        void PopulatePixelFormat(DDS_PIXELFORMAT& pf)
        {
            const auto* format = m_texture->GetFormat();

            switch (format->GetType())
            {
            case ImageFormatType::BLOCK_COMPRESSED:
                PopulatePixelFormatBlockCompressed(pf, dynamic_cast<const ImageFormatBlockCompressed*>(format));
                break;
            case ImageFormatType::UNSIGNED:
                PopulatePixelFormatUnsigned(pf, dynamic_cast<const ImageFormatUnsigned*>(format));
                break;
            default:
                assert(false);
                break;
            }
        }

        void PopulateDdsHeader(DDS_HEADER& header)
        {
            header.dwSize = sizeof(header);
            header.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

            if (m_texture->HasMipMaps())
                header.dwFlags |= DDSD_MIPMAPCOUNT;

            if (m_texture->GetFormat()->GetType() == ImageFormatType::BLOCK_COMPRESSED)
                header.dwFlags |= DDSD_LINEARSIZE;
            else
                header.dwFlags |= DDSD_PITCH;

            if (m_texture->GetDepth() > 1)
                header.dwFlags |= DDSD_DEPTH;

            header.dwHeight = m_texture->GetHeight();
            header.dwWidth = m_texture->GetWidth();
            header.dwDepth = m_texture->GetDepth();
            header.dwPitchOrLinearSize = static_cast<uint32_t>(m_texture->GetFormat()->GetPitch(0, m_texture->GetWidth()));
            header.dwMipMapCount = m_texture->HasMipMaps() ? m_texture->GetMipMapCount() : 1;

            PopulatePixelFormat(header.ddspf);

            header.dwCaps = DDSCAPS_TEXTURE;

            if (m_texture->HasMipMaps())
                header.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;

            if (m_texture->GetTextureType() == TextureType::T_CUBE)
                header.dwCaps |= DDSCAPS_COMPLEX;

            header.dwCaps2 = 0;

            if (m_texture->GetTextureType() == TextureType::T_CUBE)
            {
                header.dwCaps2 |= DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY
                                  | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ;
            }

            header.dwCaps3 = 0;
            header.dwCaps4 = 0;
            header.dwReserved2 = 0;
        }

        void PopulateDxt10Header(DDS_HEADER_DXT10& header) const
        {
            header.dxgiFormat = m_texture->GetFormat()->GetDxgiFormat();
            header.miscFlag = 0;
            header.miscFlags2 = 0;

            switch (m_texture->GetTextureType())
            {
            case TextureType::T_2D:
                header.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
                header.arraySize = 1;
                break;
            case TextureType::T_CUBE:
                header.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
                header.arraySize = 6;
                header.miscFlag |= DDS_RESOURCE_MISC_TEXTURECUBE;
                break;
            case TextureType::T_3D:
                header.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE3D;
                header.arraySize = 1;
                break;
            }
        }

        std::ostream& m_stream;
        const Texture* m_texture;
        bool m_use_dx10_extension;
    };
} // namespace

namespace image
{
    std::unique_ptr<Texture> ConvertTextureForDdsFileOutputIfNecessary(const Texture* texture)
    {
        static const std::unordered_map<ImageFormatId, ImageFormatId> ddsConversionTable{
            {ImageFormatId::R8_G8_B8, ImageFormatId::B8_G8_R8_X8},
        };

        const auto entry = ddsConversionTable.find(texture->GetFormat()->GetId());

        if (entry != ddsConversionTable.end())
        {
            TextureConverter converter(texture, ImageFormat::GetImageFormatById(entry->second));
            return converter.Convert();
        }

        return nullptr;
    }

    bool DdsWriter::SupportsImageFormat(const ImageFormat* imageFormat)
    {
        return true;
    }

    std::string DdsWriter::GetFileExtension()
    {
        return ".dds";
    }

    void DdsWriter::DumpImage(std::ostream& stream, const Texture* texture)
    {
        DdsWriterInternal internal(stream, texture);
        internal.DumpImage();
    }
} // namespace image
