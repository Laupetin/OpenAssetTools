#include "IwiLoader.h"

#include <cassert>
#include <type_traits>

#include "Image/IwiTypes.h"

IwiLoader::IwiLoader(MemoryManager* memoryManager)
{
    m_memory_manager = memoryManager;
}

const ImageFormat* IwiLoader::GetFormat6(int8_t format)
{
    switch (static_cast<iwi6::IwiFormat>(format))
    {
    case iwi6::IwiFormat::IMG_FORMAT_BITMAP_RGBA:
        return &ImageFormat::FORMAT_R8_G8_B8_A8;
    case iwi6::IwiFormat::IMG_FORMAT_BITMAP_RGB:
        return &ImageFormat::FORMAT_R8_G8_B8;
    case iwi6::IwiFormat::IMG_FORMAT_BITMAP_ALPHA:
        return &ImageFormat::FORMAT_A8;
    case iwi6::IwiFormat::IMG_FORMAT_DXT1:
        return &ImageFormat::FORMAT_BC1;
    case iwi6::IwiFormat::IMG_FORMAT_DXT3:
        return &ImageFormat::FORMAT_BC2;
    case iwi6::IwiFormat::IMG_FORMAT_DXT5:
        return &ImageFormat::FORMAT_BC3;
    case iwi6::IwiFormat::IMG_FORMAT_DXN:
        return &ImageFormat::FORMAT_BC5;
    case iwi6::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
        return &ImageFormat::FORMAT_R8_A8;
    case iwi6::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE:
        return &ImageFormat::FORMAT_R8;
    case iwi6::IwiFormat::IMG_FORMAT_WAVELET_RGBA: // used
    case iwi6::IwiFormat::IMG_FORMAT_WAVELET_RGB: // used
    case iwi6::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
    case iwi6::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE:
    case iwi6::IwiFormat::IMG_FORMAT_WAVELET_ALPHA:
        printf("Unsupported IWI format: %i\n", format);
        break;
    default:
        printf("Unknown IWI format: %i\n", format);
        break;
    }

    return nullptr;
}

Texture* IwiLoader::LoadIwi6(std::istream& stream) const
{
    iwi6::IwiHeader header{};

    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (stream.gcount() != sizeof(header))
        return nullptr;

    const auto* format = GetFormat6(header.format);
    if (format == nullptr)
        return nullptr;

    auto width = header.dimensions[0];
    auto height = header.dimensions[1];
    auto depth = header.dimensions[2];
    auto hasMipMaps = !(header.flags & iwi6::IwiFlags::IMG_FLAG_NOMIPMAPS);

    Texture* texture;
    if (header.flags & iwi6::IwiFlags::IMG_FLAG_CUBEMAP)
    {
        texture = m_memory_manager->Create<TextureCube>(format, width, height, hasMipMaps);
    }
    else if (header.flags & iwi6::IwiFlags::IMG_FLAG_VOLMAP)
    {
        texture = m_memory_manager->Create<Texture3D>(format, width, height, depth, hasMipMaps);
    }
    else
    {
        texture = m_memory_manager->Create<Texture2D>(format, width, height, hasMipMaps);
    }

    texture->Allocate();

    auto currentFileSize = sizeof(iwi6::IwiHeader) + sizeof(IwiVersion);
    const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

    for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        currentFileSize += sizeOfMipLevel;

        if (currentMipLevel < static_cast<int>(std::extent<decltype(iwi6::IwiHeader::fileSizeForPicmip)>::value)
            && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
        {
            printf("Iwi has invalid file size for picmip %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }

        stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
        if (stream.gcount() != sizeOfMipLevel)
        {
            printf("Unexpected eof of iwi in mip level %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }
    }

    return texture;
}

const ImageFormat* IwiLoader::GetFormat8(int8_t format)
{
    switch (static_cast<iwi8::IwiFormat>(format))
    {
    case iwi8::IwiFormat::IMG_FORMAT_BITMAP_RGBA:
        return &ImageFormat::FORMAT_R8_G8_B8_A8;
    case iwi8::IwiFormat::IMG_FORMAT_BITMAP_RGB:
        return &ImageFormat::FORMAT_R8_G8_B8;
    case iwi8::IwiFormat::IMG_FORMAT_BITMAP_ALPHA:
        return &ImageFormat::FORMAT_A8;
    case iwi8::IwiFormat::IMG_FORMAT_DXT1:
        return &ImageFormat::FORMAT_BC1;
    case iwi8::IwiFormat::IMG_FORMAT_DXT3:
        return &ImageFormat::FORMAT_BC2;
    case iwi8::IwiFormat::IMG_FORMAT_DXT5:
        return &ImageFormat::FORMAT_BC3;
    case iwi8::IwiFormat::IMG_FORMAT_DXN:
        return &ImageFormat::FORMAT_BC5;
    case iwi8::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
        return &ImageFormat::FORMAT_R8_A8;
    case iwi8::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE:
        return &ImageFormat::FORMAT_R8;
    case iwi8::IwiFormat::IMG_FORMAT_WAVELET_RGBA: // used
    case iwi8::IwiFormat::IMG_FORMAT_WAVELET_RGB: // used
    case iwi8::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE:
    case iwi8::IwiFormat::IMG_FORMAT_WAVELET_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_DXT3A_AS_LUMINANCE:
    case iwi8::IwiFormat::IMG_FORMAT_DXT5A_AS_LUMINANCE:
    case iwi8::IwiFormat::IMG_FORMAT_DXT3A_AS_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_DXT5A_AS_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_DXT1_AS_LUMINANCE_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_DXN_AS_LUMINANCE_ALPHA:
    case iwi8::IwiFormat::IMG_FORMAT_DXT1_AS_LUMINANCE:
    case iwi8::IwiFormat::IMG_FORMAT_DXT1_AS_ALPHA:
        printf("Unsupported IWI format: %i\n", format);
        break;
    default:
        printf("Unknown IWI format: %i\n", format);
        break;
    }

    return nullptr;
}

Texture* IwiLoader::LoadIwi8(std::istream& stream) const
{
    iwi8::IwiHeader header{};

    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (stream.gcount() != sizeof(header))
        return nullptr;

    const auto* format = GetFormat8(header.format);
    if (format == nullptr)
        return nullptr;

    auto width = header.dimensions[0];
    auto height = header.dimensions[1];
    auto depth = header.dimensions[2];
    auto hasMipMaps = !(header.flags & iwi8::IwiFlags::IMG_FLAG_NOMIPMAPS);

    Texture* texture;
    if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_CUBE)
    {
        texture = m_memory_manager->Create<TextureCube>(format, width, height, hasMipMaps);
    }
    else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_3D)
    {
        texture = m_memory_manager->Create<Texture3D>(format, width, height, depth, hasMipMaps);
    }
    else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_2D)
    {
        texture = m_memory_manager->Create<Texture2D>(format, width, height, hasMipMaps);
    }
    else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_1D)
    {
        printf("Iwi has unsupported map type 1D\n");
        return nullptr;
    }
    else
    {
        printf("Iwi has unsupported map type\n");
        return nullptr;
    }

    texture->Allocate();

    auto currentFileSize = sizeof(iwi8::IwiHeader) + sizeof(IwiVersion);
    const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

    for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        currentFileSize += sizeOfMipLevel;

        if (currentMipLevel < static_cast<int>(std::extent<decltype(iwi8::IwiHeader::fileSizeForPicmip)>::value)
            && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
        {
            printf("Iwi has invalid file size for picmip %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }

        stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
        if (stream.gcount() != sizeOfMipLevel)
        {
            printf("Unexpected eof of iwi in mip level %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }
    }

    return texture;
}

const ImageFormat* IwiLoader::GetFormat27(int8_t format)
{
    switch (static_cast<iwi27::IwiFormat>(format))
    {
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGBA:
        return &ImageFormat::FORMAT_R8_G8_B8_A8;
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_ALPHA:
        return &ImageFormat::FORMAT_A8;
    case iwi27::IwiFormat::IMG_FORMAT_DXT1:
        return &ImageFormat::FORMAT_BC1;
    case iwi27::IwiFormat::IMG_FORMAT_DXT3:
        return &ImageFormat::FORMAT_BC2;
    case iwi27::IwiFormat::IMG_FORMAT_DXT5:
        return &ImageFormat::FORMAT_BC3;
    case iwi27::IwiFormat::IMG_FORMAT_DXN:
        return &ImageFormat::FORMAT_BC5;
    case iwi27::IwiFormat::IMG_FORMAT_A16B16G16R16F:
        assert(false); // Unsupported yet
        return &ImageFormat::FORMAT_R16_G16_B16_A16_FLOAT;
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGB:
        return &ImageFormat::FORMAT_R8_G8_B8;
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
        return &ImageFormat::FORMAT_R8_A8;
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE:
        return &ImageFormat::FORMAT_R8;
    case iwi27::IwiFormat::IMG_FORMAT_WAVELET_RGBA:
    case iwi27::IwiFormat::IMG_FORMAT_WAVELET_RGB:
    case iwi27::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
    case iwi27::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE:
    case iwi27::IwiFormat::IMG_FORMAT_WAVELET_ALPHA:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGB565:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGB5A3:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_C8:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGBA8:
        printf("Unsupported IWI format: %i\n", format);
        break;
    default:
        printf("Unknown IWI format: %i\n", format);
        break;
    }

    return nullptr;
}

Texture* IwiLoader::LoadIwi27(std::istream& stream) const
{
    iwi27::IwiHeader header{};

    stream.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (stream.gcount() != sizeof(header))
        return nullptr;

    const auto* format = GetFormat27(header.format);
    if (format == nullptr)
        return nullptr;

    auto width = header.dimensions[0];
    auto height = header.dimensions[1];
    auto depth = header.dimensions[2];
    auto hasMipMaps = !(header.flags & iwi27::IwiFlags::IMG_FLAG_NOMIPMAPS);

    Texture* texture;
    if (header.flags & iwi27::IwiFlags::IMG_FLAG_CUBEMAP)
    {
        texture = m_memory_manager->Create<TextureCube>(format, width, height, hasMipMaps);
    }
    else if (header.flags & iwi27::IwiFlags::IMG_FLAG_VOLMAP)
    {
        texture = m_memory_manager->Create<Texture3D>(format, width, height, depth, hasMipMaps);
    }
    else
    {
        texture = m_memory_manager->Create<Texture2D>(format, width, height, hasMipMaps);
    }

    texture->Allocate();

    auto currentFileSize = sizeof(iwi27::IwiHeader) + sizeof(IwiVersion);
    const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

    for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        currentFileSize += sizeOfMipLevel;

        if (currentMipLevel < static_cast<int>(std::extent<decltype(iwi27::IwiHeader::fileSizeForPicmip)>::value)
            && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
        {
            printf("Iwi has invalid file size for picmip %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }

        stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
        if (stream.gcount() != sizeOfMipLevel)
        {
            printf("Unexpected eof of iwi in mip level %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }
    }

    return texture;
}

Texture* IwiLoader::LoadIwi(std::istream& stream)
{
    IwiVersion iwiVersion{};

    stream.read(reinterpret_cast<char*>(&iwiVersion), sizeof(iwiVersion));
    if (stream.gcount() != sizeof(iwiVersion))
        return nullptr;

    if (iwiVersion.tag[0] != 'I'
        || iwiVersion.tag[1] != 'W'
        || iwiVersion.tag[2] != 'i')
    {
        printf("Invalid IWI magic\n");
    }

    switch (iwiVersion.version)
    {
    case 6:
        return LoadIwi6(stream);

    case 8:
        return LoadIwi8(stream);

    case 27:
        return LoadIwi27(stream);

    default:
        break;
    }

    printf("Unknown IWI version %i\n", iwiVersion.version);
    return nullptr;
}
