#include "DdsWriter.h"

#include <cassert>

#include "Image/DdsTypes.h"

using namespace dds;

class DdsWriterInternal
{
    static constexpr uint32_t DDS_MAGIC = MakeFourCc('D', 'D', 'S', ' ');
    static constexpr uint32_t DDS_PF_DXT10_EXTENSION = MakeFourCc('D', 'X', '1', '0');
    static constexpr uint32_t DDS_PF_BC1 = MakeFourCc('D', 'X', 'T', '1');
    static constexpr uint32_t DDS_PF_BC2 = MakeFourCc('D', 'X', 'T', '3');
    static constexpr uint32_t DDS_PF_BC3 = MakeFourCc('D', 'X', 'T', '5');

public:
    enum class ImageFormatSupportStatus
    {
        UNSUPPORTED,
        SUPPORTED_WITH_SIMPLE_HEADER,
        SUPPORTED_WITH_DXT10_HEADER
    };

private:
    FileAPI::IFile* m_file;
    Texture* m_texture;
    ImageFormatSupportStatus m_support_status;

    void PrepareHeader(DDS_HEADER* header) const
    {
        header->dwSize = sizeof DDS_HEADER;
        header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

        if (m_texture->HasMipMaps())
            header->dwFlags |= DDSD_MIPMAPCOUNT;

        if (m_texture->GetFormat()->GetType() == ImageFormatType::BLOCK_COMPRESSED)
            header->dwFlags |= DDSD_LINEARSIZE;
        else
            header->dwFlags |= DDSD_PITCH;

        if (m_texture->GetType() == Texture::Type::TYPE_3D)
            header->dwFlags |= DDSD_DEPTH;

        header->dwWidth = m_texture->GetWidth();
        header->dwHeight = m_texture->GetHeight();
        header->dwDepth = m_texture->GetDepth();
        header->dwPitchOrLinearSize = m_texture->GetFormat()->GetPitch(m_texture->GetWidth());
        header->dwMipMapCount = m_texture->HasMipMaps() ? m_texture->GetMipMapCount() : 1;

        memset(header->dwReserved1, 0, sizeof DDS_HEADER::dwReserved1);

        header->dwCaps = DDSCAPS_TEXTURE;
        if (m_texture->HasMipMaps())
            header->dwCaps |= DDSCAPS_MIPMAP;
        if (m_texture->GetType() == Texture::Type::TYPE_CUBE || m_texture->HasMipMaps())
            header->dwCaps |= DDSCAPS_COMPLEX;

        header->dwCaps2 = 0;
        if (m_texture->GetType() == Texture::Type::TYPE_CUBE)
        {
            header->dwCaps2 |=
                DDSCAPS2_CUBEMAP
                | DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX
                | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY
                | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ;
        }
        else if (m_texture->GetType() == Texture::Type::TYPE_3D)
        {
            header->dwCaps2 |= DDSCAPS2_VOLUME;
        }

        header->dwCaps3 = 0;
        header->dwCaps4 = 0;
        header->dwReserved2 = 0;
    }

    void SetPixelFormat(DDS_PIXELFORMAT* pixelFormat) const
    {
        pixelFormat->dwSize = sizeof DDS_PIXELFORMAT;
        pixelFormat->dwFlags = 0;

        const auto* imageFormat = m_texture->GetFormat();
        switch (imageFormat->GetType())
        {
        case ImageFormatType::UNSIGNED:
            {
                const auto* imageFormatUnsigned = dynamic_cast<const ImageFormatUnsigned*>(imageFormat);

                if (imageFormatUnsigned->m_r_size == 0
                    && imageFormatUnsigned->m_g_size == 0
                    && imageFormatUnsigned->m_b_size == 0
                    && imageFormatUnsigned->m_a_size > 0)
                {
                    pixelFormat->dwFlags |= DDPF_ALPHA;
                }
                else
                {
                    if (imageFormatUnsigned->m_r_size > 0
                        && imageFormatUnsigned->m_g_size == 0
                        && imageFormatUnsigned->m_b_size == 0)
                    {
                        pixelFormat->dwFlags |= DDPF_LUMINANCE;
                    }
                    else if (imageFormatUnsigned->m_r_size > 0
                        || imageFormatUnsigned->m_g_size > 0
                        || imageFormatUnsigned->m_b_size > 0)
                    {
                        pixelFormat->dwFlags |= DDPF_RGB;
                    }

                    if (imageFormatUnsigned->m_a_size > 0)
                    {
                        pixelFormat->dwFlags |= DDPF_ALPHAPIXELS;
                    }
                }

                pixelFormat->dwRGBBitCount = imageFormatUnsigned->m_bits_per_pixel;
                pixelFormat->dwRBitMask = static_cast<uint32_t>(imageFormatUnsigned->m_r_mask);
                pixelFormat->dwGBitMask = static_cast<uint32_t>(imageFormatUnsigned->m_g_mask);
                pixelFormat->dwBBitMask = static_cast<uint32_t>(imageFormatUnsigned->m_b_mask);
                pixelFormat->dwABitMask = static_cast<uint32_t>(imageFormatUnsigned->m_a_mask);
            }
            break;
        case ImageFormatType::BLOCK_COMPRESSED:
            {
                pixelFormat->dwFlags |= DDPF_FOURCC;
                pixelFormat->dwRGBBitCount = 0;
                pixelFormat->dwRBitMask = 0;
                pixelFormat->dwGBitMask = 0;
                pixelFormat->dwBBitMask = 0;
                pixelFormat->dwABitMask = 0;

                switch (imageFormat->GetId())
                {
                case ImageFormatId::BC1:
                    pixelFormat->dwFourCC = DDS_PF_BC1;
                    break;
                case ImageFormatId::BC2:
                    pixelFormat->dwFourCC = DDS_PF_BC2;
                    break;
                case ImageFormatId::BC3:
                    pixelFormat->dwFourCC = DDS_PF_BC3;
                    break;
                default:
                    assert(false);
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

    static void MarkHeaderAsExtendedByHeaderDxt10(DDS_PIXELFORMAT* pixelFormat)
    {
        pixelFormat->dwSize = sizeof DDS_PIXELFORMAT;
        pixelFormat->dwFlags = DDPF_FOURCC;
        pixelFormat->dwFourCC = DDS_PF_DXT10_EXTENSION;
        pixelFormat->dwRGBBitCount = 0;
        pixelFormat->dwRBitMask = 0;
        pixelFormat->dwGBitMask = 0;
        pixelFormat->dwBBitMask = 0;
        pixelFormat->dwABitMask = 0;
    }

    void PrepareHeaderDxt10(DDS_HEADER_DXT10* header)
    {
        header->dxgiFormat = m_texture->GetFormat()->GetDxgiFormat();

        switch (m_texture->GetType())
        {
        case Texture::Type::TYPE_2D:
        case Texture::Type::TYPE_CUBE:
            header->resourceDimension = DDS_DIMENSION_TEXTURE2D;
            break;
        case Texture::Type::TYPE_3D:
            header->resourceDimension = DDS_DIMENSION_TEXTURE3D;
            break;
        }

        header->miscFlag = 0;
        if (m_texture->GetType() == Texture::Type::TYPE_CUBE)
            header->miscFlag |= DDS_RESOURCE_MISC_TEXTURECUBE;

        header->arraySize = m_texture->GetFaceCount();
        header->miscFlags2 = 0;
    }

public:
    static ImageFormatSupportStatus GetSupportStatusForImageFormat(const ImageFormat* format)
    {
        assert(format != nullptr);

        if (auto* imageFormatUnsigned = dynamic_cast<const ImageFormatUnsigned*>(format))
        {
            if (imageFormatUnsigned->m_bits_per_pixel <= sizeof uint32_t * 8)
                return ImageFormatSupportStatus::SUPPORTED_WITH_SIMPLE_HEADER;
        }
        else
        {
            switch (format->GetId())
            {
            case ImageFormatId::BC1:
            case ImageFormatId::BC2:
            case ImageFormatId::BC3:
                return ImageFormatSupportStatus::SUPPORTED_WITH_SIMPLE_HEADER;
            default:
                break;
            }
        }

        return format->GetDxgiFormat() != DXGI_FORMAT_UNKNOWN
                   ? ImageFormatSupportStatus::SUPPORTED_WITH_DXT10_HEADER
                   : ImageFormatSupportStatus::UNSUPPORTED;
    }

    DdsWriterInternal(FileAPI::IFile* file, Texture* texture)
    {
        m_file = file;
        m_texture = texture;
        m_support_status = GetSupportStatusForImageFormat(texture->GetFormat());
    }

    bool DumpImage()
    {
        if (m_file->Write(&DDS_MAGIC, sizeof uint32_t, 1) != 1)
            return false;

        DDS_HEADER ddsHeader{};
        PrepareHeader(&ddsHeader);
        if (m_support_status == ImageFormatSupportStatus::SUPPORTED_WITH_SIMPLE_HEADER)
        {
            SetPixelFormat(&ddsHeader.ddspf);

            if (m_file->Write(&ddsHeader, sizeof DDS_HEADER, 1) != 1)
                return false;
        }
        else if (m_support_status == ImageFormatSupportStatus::SUPPORTED_WITH_DXT10_HEADER)
        {
            DDS_HEADER_DXT10 ddsHeaderDxt10{};

            MarkHeaderAsExtendedByHeaderDxt10(&ddsHeader.ddspf);
            PrepareHeaderDxt10(&ddsHeaderDxt10);

            if (m_file->Write(&ddsHeader, sizeof DDS_HEADER, 1) != 1
                || m_file->Write(&ddsHeaderDxt10, sizeof DDS_HEADER_DXT10, 1) != 1)
                return false;
        }

        const int mipCount = m_texture->HasMipMaps() ? m_texture->GetMipMapCount() : 1;
        for (int face = 0; face < m_texture->GetFaceCount(); face++)
        {
            for (int mipLevel = 0; mipLevel < mipCount; mipLevel++)
            {
                const size_t sizeOfMipLevel = m_texture->GetSizeOfMipLevel(mipLevel);

                if (m_file->Write(m_texture->GetBufferForMipLevel(mipLevel, face), 1, sizeOfMipLevel) != sizeOfMipLevel)
                    return false;
            }
        }

        return true;
    }
};

DdsWriter::~DdsWriter()
= default;

bool DdsWriter::SupportsImageFormat(const ImageFormat* imageFormat)
{
    return DdsWriterInternal::GetSupportStatusForImageFormat(imageFormat)
        != DdsWriterInternal::ImageFormatSupportStatus::UNSUPPORTED;
}

std::string DdsWriter::GetFileExtension()
{
    return ".dds";
}

void DdsWriter::DumpImage(FileAPI::IFile* file, Texture* texture)
{
    DdsWriterInternal writer(file, texture);
    writer.DumpImage();
}
