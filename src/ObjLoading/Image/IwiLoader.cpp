#include "IwiLoader.h"
#include "Image/IwiTypes.h"
#include <cassert>

IwiLoader::IwiLoader(MemoryManager* memoryManager)
{
    m_memory_manager = memoryManager;
}

const IImageFormat* IwiLoader::GetFormat27(int8_t format)
{
    switch (static_cast<iwi27::IwiFormat>(format))
    {
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGBA:
        return &ImageFormat::FORMAT_R8G8B8A8;
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
        return &ImageFormat::FORMAT_R16G16B16A16_FLOAT;
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGB:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
    case iwi27::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE:
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

Texture* IwiLoader::LoadIwi27(FileAPI::IFile* file)
{
    iwi27::IwiHeader header{};

    if (file->Read(&header, sizeof header, 1) != 1)
        return nullptr;

    const IImageFormat* format = GetFormat27(header.format);
    if (format == nullptr)
        return nullptr;

    uint16_t width = header.dimensions[0];
    uint16_t height = header.dimensions[1];
    uint16_t depth = header.dimensions[2];
    bool hasMipMaps = !(header.flags & iwi27::IwiFlags::IMG_FLAG_NOMIPMAPS);

    Texture* texture;
    if(header.flags & iwi27::IwiFlags::IMG_FLAG_CUBEMAP)
    {
        texture = m_memory_manager->Create<TextureCube>(format, width, height, hasMipMaps);
    }
    else if(header.flags & iwi27::IwiFlags::IMG_FLAG_VOLMAP)
    {
        texture = m_memory_manager->Create<Texture3D>(format, width, height, depth, hasMipMaps);
    }
    else
    {
        texture = m_memory_manager->Create<Texture2D>(format, width, height, hasMipMaps);
    }

    texture->Allocate();

    size_t currentFileSize = sizeof iwi27::IwiHeader + sizeof IwiVersion;
    const int mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

    for (int currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const size_t sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel);
        currentFileSize += sizeOfMipLevel;

        if (currentMipLevel < static_cast<int>(_countof(iwi27::IwiHeader::fileSizeForPicmip))
            && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
        {
            printf("Iwi has invalid file size for picmip %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }

        if (file->Read(texture->GetBufferForMipLevel(currentMipLevel), 1, sizeOfMipLevel) != sizeOfMipLevel)
        {
            printf("Unexpected eof of iwi in mip level %i\n", currentMipLevel);

            m_memory_manager->Delete(texture);
            return nullptr;
        }
    }

    return texture;
}

Texture* IwiLoader::LoadIwi(FileAPI::IFile* file)
{
    IwiVersion iwiVersion{};

    if (file->Read(&iwiVersion, sizeof iwiVersion, 1) != 1)
        return nullptr;

    if (iwiVersion.tag[0] != 'I'
        || iwiVersion.tag[1] != 'W'
        || iwiVersion.tag[2] != 'i')
    {
        printf("Invalid IWI magic\n");
    }

    switch (iwiVersion.version)
    {
    case 27:
        return LoadIwi27(file);

    default:
        break;
    }

    printf("Unknown IWI version %i\n", iwiVersion.version);
    return nullptr;
}
