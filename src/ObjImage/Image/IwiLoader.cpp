#include "IwiLoader.h"

#include "Image/IwiTypes.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <format>
#include <iostream>
#include <type_traits>

namespace iwi
{
    const ImageFormat* GetFormat6(int8_t format)
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
        case iwi6::IwiFormat::IMG_FORMAT_WAVELET_RGB:  // used
        case iwi6::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
        case iwi6::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE:
        case iwi6::IwiFormat::IMG_FORMAT_WAVELET_ALPHA:
            con::error("Unsupported IWI format: {}", format);
            break;
        default:
            con::error("Unknown IWI format: {}", format);
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Texture> LoadIwi6(std::istream& stream)
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

        std::unique_ptr<Texture> texture;
        if (header.flags & iwi6::IwiFlags::IMG_FLAG_CUBEMAP)
            texture = std::make_unique<TextureCube>(format, width, height, hasMipMaps);
        else if (header.flags & iwi6::IwiFlags::IMG_FLAG_VOLMAP)
            texture = std::make_unique<Texture3D>(format, width, height, depth, hasMipMaps);
        else
            texture = std::make_unique<Texture2D>(format, width, height, hasMipMaps);

        texture->Allocate();

        auto currentFileSize = sizeof(iwi6::IwiHeader) + sizeof(IwiVersion);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi6::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return nullptr;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return nullptr;
            }
        }

        return texture;
    }

    const ImageFormat* GetFormat8(int8_t format)
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
        case iwi8::IwiFormat::IMG_FORMAT_WAVELET_RGB:  // used
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
            con::error("Unsupported IWI format: {}", format);
            break;
        default:
            con::error("Unknown IWI format: {}", format);
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Texture> LoadIwi8(std::istream& stream)
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

        std::unique_ptr<Texture> texture;
        if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_CUBE)
        {
            texture = std::make_unique<TextureCube>(format, width, height, hasMipMaps);
        }
        else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_3D)
        {
            texture = std::make_unique<Texture3D>(format, width, height, depth, hasMipMaps);
        }
        else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_2D)
        {
            texture = std::make_unique<Texture2D>(format, width, height, hasMipMaps);
        }
        else if ((header.flags & iwi8::IwiFlags::IMG_FLAG_MAPTYPE_MASK) == iwi8::IwiFlags::IMG_FLAG_MAPTYPE_1D)
        {
            con::error("Iwi has unsupported map type 1D");
            return nullptr;
        }
        else
        {
            con::error("Iwi has unsupported map type");
            return nullptr;
        }

        texture->Allocate();

        auto currentFileSize = sizeof(iwi8::IwiHeader) + sizeof(IwiVersion);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi8::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return nullptr;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return nullptr;
            }
        }

        return texture;
    }

    const ImageFormat* GetFormat13(int8_t format)
    {
        switch (static_cast<iwi13::IwiFormat>(format))
        {
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_RGBA:
            return &ImageFormat::FORMAT_R8_G8_B8_A8;
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_RGB:
            return &ImageFormat::FORMAT_R8_G8_B8;
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_ALPHA:
            return &ImageFormat::FORMAT_A8;
        case iwi13::IwiFormat::IMG_FORMAT_DXT1:
            return &ImageFormat::FORMAT_BC1;
        case iwi13::IwiFormat::IMG_FORMAT_DXT3:
            return &ImageFormat::FORMAT_BC2;
        case iwi13::IwiFormat::IMG_FORMAT_DXT5:
            return &ImageFormat::FORMAT_BC3;
        case iwi13::IwiFormat::IMG_FORMAT_DXN:
            return &ImageFormat::FORMAT_BC5;
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE_ALPHA:
            return &ImageFormat::FORMAT_R8_A8;
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_LUMINANCE:
            return &ImageFormat::FORMAT_R8;
        case iwi13::IwiFormat::IMG_FORMAT_WAVELET_RGBA: // used
        case iwi13::IwiFormat::IMG_FORMAT_WAVELET_RGB:  // used
        case iwi13::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE_ALPHA:
        case iwi13::IwiFormat::IMG_FORMAT_WAVELET_LUMINANCE:
        case iwi13::IwiFormat::IMG_FORMAT_WAVELET_ALPHA:
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_RGB565:
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_RGB5A3:
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_C8:
        case iwi13::IwiFormat::IMG_FORMAT_BITMAP_RGBA8:
        case iwi13::IwiFormat::IMG_FORMAT_A16B16G16R16F:
            con::error("Unsupported IWI format: {}", format);
            break;
        default:
            con::error("Unknown IWI format: {}", format);
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Texture> LoadIwi13(std::istream& stream)
    {
        iwi13::IwiHeader header{};

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header))
            return nullptr;

        const auto* format = GetFormat6(header.format);
        if (format == nullptr)
            return nullptr;

        auto width = header.dimensions[0];
        auto height = header.dimensions[1];
        auto depth = header.dimensions[2];
        auto hasMipMaps = !(header.flags & iwi13::IwiFlags::IMG_FLAG_NOMIPMAPS);

        std::unique_ptr<Texture> texture;
        if (header.flags & iwi13::IwiFlags::IMG_FLAG_CUBEMAP)
            texture = std::make_unique<TextureCube>(format, width, height, hasMipMaps);
        else if (header.flags & iwi13::IwiFlags::IMG_FLAG_VOLMAP)
            texture = std::make_unique<Texture3D>(format, width, height, depth, hasMipMaps);
        else
            texture = std::make_unique<Texture2D>(format, width, height, hasMipMaps);

        texture->Allocate();

        auto currentFileSize = sizeof(iwi13::IwiHeader) + sizeof(IwiVersion);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi13::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return nullptr;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return nullptr;
            }
        }

        return texture;
    }

    const ImageFormat* GetFormat27(int8_t format)
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
            con::error("Unsupported IWI format: {}", format);
            break;
        default:
            con::error("Unknown IWI format: {}", format);
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Texture> LoadIwi27(std::istream& stream)
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

        std::unique_ptr<Texture> texture;
        if (header.flags & iwi27::IwiFlags::IMG_FLAG_CUBEMAP)
            texture = std::make_unique<TextureCube>(format, width, height, hasMipMaps);
        else if (header.flags & iwi27::IwiFlags::IMG_FLAG_VOLMAP)
            texture = std::make_unique<Texture3D>(format, width, height, depth, hasMipMaps);
        else
            texture = std::make_unique<Texture2D>(format, width, height, hasMipMaps);

        texture->Allocate();

        auto currentFileSize = sizeof(iwi27::IwiHeader) + sizeof(IwiVersion);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi27::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return nullptr;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return nullptr;
            }
        }

        return texture;
    }

    std::unique_ptr<Texture> LoadIwi(std::istream& stream)
    {
        IwiVersion iwiVersion{};

        stream.read(reinterpret_cast<char*>(&iwiVersion), sizeof(iwiVersion));
        if (stream.gcount() != sizeof(iwiVersion))
            return nullptr;

        if (iwiVersion.tag[0] != 'I' || iwiVersion.tag[1] != 'W' || iwiVersion.tag[2] != 'i')
        {
            con::error("Invalid IWI magic");
            return nullptr;
        }

        switch (iwiVersion.version)
        {
        case 6:
            return LoadIwi6(stream);

        case 8:
            return LoadIwi8(stream);

        case 13:
            return LoadIwi13(stream);

        case 27:
            return LoadIwi27(stream);

        default:
            break;
        }

        con::error("Unknown IWI version {}", iwiVersion.version);
        return nullptr;
    }
} // namespace iwi
