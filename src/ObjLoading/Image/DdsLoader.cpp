#include "DdsLoader.h"

#include "Image/DdsTypes.h"
#include "Utils/ClassUtils.h"
#include "Utils/FileUtils.h"

#include <iostream>

class DdsLoaderInternal
{
    static constexpr auto DDS_MAGIC = FileUtils::MakeMagic32('D', 'D', 'S', ' ');

    MemoryManager* m_memory_manager;
    std::istream& m_stream;

    TextureType m_texture_type;
    bool m_has_mip_maps;
    size_t m_width;
    size_t m_height;
    size_t m_depth;
    const ImageFormat* m_format;

    _NODISCARD bool ReadMagic() const
    {
        uint32_t magic;
        m_stream.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        if (m_stream.gcount() != sizeof(magic))
        {
            std::cout << "Failed to read dds data\n";
            return false;
        }

        if (magic != DDS_MAGIC)
        {
            std::cout << "Invalid magic for dds\n";
            return false;
        }

        return true;
    }

    _NODISCARD bool ReadDxt10Header()
    {
        DDS_HEADER_DXT10 headerDx10{};
        m_stream.read(reinterpret_cast<char*>(&headerDx10), sizeof(headerDx10));
        if (m_stream.gcount() != sizeof(headerDx10))
        {
            std::cout << "Failed to read dds data\n";
            return false;
        }

        if (headerDx10.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE3D)
        {
            m_texture_type = TextureType::T_3D;
        }
        else if (headerDx10.resourceDimension == D3D10_RESOURCE_DIMENSION_TEXTURE2D)
        {
            if (headerDx10.miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE || headerDx10.arraySize == 6)
            {
                m_texture_type = TextureType::T_CUBE;
            }
            else
            {
                m_texture_type = TextureType::T_2D;
            }
        }
        else
        {
            std::cout << "Unsupported dds resourceDimension " << headerDx10.resourceDimension << "\n";
            return false;
        }

        for (const auto* imageFormat : ImageFormat::ALL_FORMATS)
        {
            if (imageFormat->GetDxgiFormat() == headerDx10.dxgiFormat)
            {
                m_format = imageFormat;
                return true;
            }
        }

        std::cout << "Unsupported dds dxgi format " << headerDx10.dxgiFormat << "\n";
        return false;
    }

    _NODISCARD bool ReadPixelFormatFourCc(DDS_PIXELFORMAT& pf)
    {
        switch (pf.dwFourCC)
        {
        case FileUtils::MakeMagic32('D', 'X', 'T', '1'):
            m_format = &ImageFormat::FORMAT_BC1;
            return true;

        case FileUtils::MakeMagic32('D', 'X', 'T', '3'):
            m_format = &ImageFormat::FORMAT_BC2;
            return true;

        case FileUtils::MakeMagic32('D', 'X', 'T', '5'):
            m_format = &ImageFormat::FORMAT_BC3;
            return true;

        case FileUtils::MakeMagic32('D', 'X', '1', '0'):
            return ReadDxt10Header();

        default:
            std::cout << "Unknown dds FourCC " << pf.dwFourCC << "\n";
            return false;
        }
    }

    static void ExtractSizeAndOffsetFromMask(uint32_t mask, unsigned& offset, unsigned& size)
    {
        offset = 0;
        size = 0;

        if (mask == 0)
            return;

        while ((mask & 1) == 0)
        {
            offset++;
            mask >>= 1;
        }

        while ((mask & 1) == 1)
        {
            size++;
            mask >>= 1;
        }
    }

    _NODISCARD bool ReadPixelFormatUnsigned(DDS_PIXELFORMAT& pf)
    {
        unsigned rOffset, rSize, gOffset, gSize, bOffset, bSize, aOffset, aSize;

        ExtractSizeAndOffsetFromMask(pf.dwRBitMask, rOffset, rSize);
        ExtractSizeAndOffsetFromMask(pf.dwGBitMask, gOffset, gSize);
        ExtractSizeAndOffsetFromMask(pf.dwBBitMask, bOffset, bSize);
        ExtractSizeAndOffsetFromMask(pf.dwABitMask, aOffset, aSize);

        for (const auto* imageFormat : ImageFormat::ALL_FORMATS)
        {
            if (imageFormat->GetType() != ImageFormatType::UNSIGNED)
                continue;

            const auto* unsignedImageFormat = dynamic_cast<const ImageFormatUnsigned*>(imageFormat);

            if (unsignedImageFormat->m_r_offset == rOffset && unsignedImageFormat->m_r_size == rSize && unsignedImageFormat->m_g_offset == gOffset
                && unsignedImageFormat->m_g_size == gSize && unsignedImageFormat->m_b_offset == bOffset && unsignedImageFormat->m_b_size == bSize
                && unsignedImageFormat->m_a_offset == aOffset && unsignedImageFormat->m_a_size == aSize)
            {
                m_format = imageFormat;
                return true;
            }
        }

        std::cout << "Failed to find dds pixel format: R=" << std::hex << pf.dwRBitMask << " G=" << std::hex << pf.dwGBitMask << " B=" << std::hex
                  << pf.dwBBitMask << " A=" << std::hex << pf.dwABitMask << "\n";

        return false;
    }

    _NODISCARD bool ReadPixelFormat(DDS_PIXELFORMAT& pf)
    {
        if (pf.dwFlags & DDPF_FOURCC)
            return ReadPixelFormatFourCc(pf);

        return ReadPixelFormatUnsigned(pf);
    }

    _NODISCARD bool ReadHeader()
    {
        DDS_HEADER header{};
        m_stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (m_stream.gcount() != sizeof(header))
        {
            std::cout << "Failed to read dds data\n";
            return false;
        }

        m_width = header.dwWidth;
        m_height = header.dwHeight;
        m_depth = header.dwDepth;
        m_has_mip_maps = (header.dwCaps & DDSCAPS_MIPMAP) != 0 || header.dwMipMapCount > 1;

        if (header.dwCaps2 & DDSCAPS2_CUBEMAP)
            m_texture_type = TextureType::T_CUBE;
        else if (header.dwDepth > 1)
            m_texture_type = TextureType::T_3D;
        else
            m_texture_type = TextureType::T_2D;

        return ReadPixelFormat(header.ddspf);
    }

    _NODISCARD Texture* ReadTextureData() const
    {
        Texture* result;

        switch (m_texture_type)
        {
        case TextureType::T_2D:
            result = new Texture2D(m_format, m_width, m_height, m_has_mip_maps);
            break;

        case TextureType::T_3D:
            result = new Texture3D(m_format, m_width, m_height, m_depth, m_has_mip_maps);
            break;

        case TextureType::T_CUBE:
            result = new TextureCube(m_format, m_width, m_height, m_has_mip_maps);
            break;

        default:
            return nullptr;
        }

        const auto mipMapCount = m_has_mip_maps ? result->GetMipMapCount() : 1;
        const auto faceCount = m_texture_type == TextureType::T_CUBE ? 6 : 1;

        result->Allocate();

        for (auto mipLevel = 0; mipLevel < mipMapCount; mipLevel++)
        {
            const auto mipSize = result->GetSizeOfMipLevel(mipLevel);

            for (auto face = 0; face < faceCount; face++)
            {
                m_stream.read(reinterpret_cast<char*>(result->GetBufferForMipLevel(mipLevel, face)), mipSize);

                if (m_stream.gcount() != mipSize)
                {
                    std::cout << "Failed to read texture data from dds\n";
                    delete result;
                    return nullptr;
                }
            }
        }

        return result;
    }

public:
    DdsLoaderInternal(MemoryManager* memoryManager, std::istream& stream)
        : m_memory_manager(memoryManager),
          m_stream(stream),
          m_texture_type(TextureType::T_2D),
          m_has_mip_maps(false),
          m_width(0u),
          m_height(0u),
          m_depth(0u),
          m_format(nullptr)
    {
    }

    Texture* LoadDds()
    {
        if (!ReadMagic() || !ReadHeader())
        {
            return nullptr;
        }

        return ReadTextureData();
    }
};

DdsLoader::DdsLoader(MemoryManager* memoryManager)
    : m_memory_manager(memoryManager)
{
}

Texture* DdsLoader::LoadDds(std::istream& stream) const
{
    DdsLoaderInternal internal(m_memory_manager, stream);
    return internal.LoadDds();
}
