#include "IwiLoader.h"

#include "Image/IwiTypes.h"
#include "Utils/Logging/Log.h"

#include <cassert>
#include <format>
#include <iostream>
#include <type_traits>

using namespace image;

namespace
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

    std::optional<IwiLoaderResult> LoadIwi6(std::istream& stream)
    {
        iwi6::IwiHeader header{};

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header))
        {
            con::error("IWI header corrupted");
            return std::nullopt;
        }

        const auto* format = GetFormat6(header.format);
        if (format == nullptr)
            return std::nullopt;

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

        auto currentFileSize = sizeof(iwi6::IwiHeader) + sizeof(IwiVersionHeader);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi6::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return std::nullopt;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return std::nullopt;
            }
        }

        CommonIwiMetaData meta{
            .m_no_picmip = (header.flags & iwi6::IwiFlags::IMG_FLAG_NOPICMIP) != 0,
            .m_streaming = (header.flags & iwi6::IwiFlags::IMG_FLAG_STREAMING) != 0,
            .m_clamp_u = (header.flags & iwi6::IwiFlags::IMG_FLAG_CLAMP_U) != 0,
            .m_clamp_v = (header.flags & iwi6::IwiFlags::IMG_FLAG_CLAMP_V) != 0,
            .m_dynamic = (header.flags & iwi6::IwiFlags::IMG_FLAG_DYNAMIC) != 0,
        };

        return IwiLoaderResult{
            .m_version = IwiVersion::IWI_6,
            .m_meta = meta,
            .m_texture = std::move(texture),
        };
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

    std::optional<IwiLoaderResult> LoadIwi8(std::istream& stream)
    {
        iwi8::IwiHeader header{};

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header))
        {
            con::error("IWI header corrupted");
            return std::nullopt;
        }

        const auto* format = GetFormat8(header.format);
        if (format == nullptr)
            return std::nullopt;

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
            return std::nullopt;
        }
        else
        {
            con::error("Iwi has unsupported map type");
            return std::nullopt;
        }

        texture->Allocate();

        auto currentFileSize = sizeof(iwi8::IwiHeader) + sizeof(IwiVersionHeader);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi8::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return std::nullopt;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return std::nullopt;
            }
        }

        CommonIwiMetaData meta{
            .m_no_picmip = (header.flags & iwi8::IwiFlags::IMG_FLAG_NOPICMIP) != 0,
            .m_streaming = (header.flags & iwi8::IwiFlags::IMG_FLAG_STREAMING) != 0,
            .m_clamp_u = (header.flags & iwi8::IwiFlags::IMG_FLAG_CLAMP_U) != 0,
            .m_clamp_v = (header.flags & iwi8::IwiFlags::IMG_FLAG_CLAMP_V) != 0,
            .m_dynamic = (header.flags & iwi8::IwiFlags::IMG_FLAG_DYNAMIC) != 0,
        };

        return IwiLoaderResult{
            .m_version = IwiVersion::IWI_8,
            .m_meta = meta,
            .m_texture = std::move(texture),
        };
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

    std::optional<IwiLoaderResult> LoadIwi13(std::istream& stream)
    {
        iwi13::IwiHeader header{};

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header))
        {
            con::error("IWI header corrupted");
            return std::nullopt;
        }

        const auto* format = GetFormat6(header.format);
        if (format == nullptr)
            return std::nullopt;

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

        auto currentFileSize = sizeof(iwi13::IwiHeader) + sizeof(IwiVersionHeader);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi13::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return std::nullopt;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return std::nullopt;
            }
        }

        CommonIwiMetaData meta{
            .m_no_picmip = (header.flags & iwi13::IwiFlags::IMG_FLAG_NOPICMIP) != 0,
            .m_streaming = (header.flags & iwi13::IwiFlags::IMG_FLAG_STREAMING) != 0,
            .m_clamp_u = (header.flags & iwi13::IwiFlags::IMG_FLAG_CLAMP_U) != 0,
            .m_clamp_v = (header.flags & iwi13::IwiFlags::IMG_FLAG_CLAMP_V) != 0,
            .m_dynamic = (header.flags & iwi13::IwiFlags::IMG_FLAG_DYNAMIC) != 0,
            .m_gamma = header.gamma,
        };

        return IwiLoaderResult{
            .m_version = IwiVersion::IWI_13,
            .m_meta = meta,
            .m_texture = std::move(texture),
        };
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

    std::optional<IwiLoaderResult> LoadIwi27(std::istream& stream)
    {
        iwi27::IwiHeader header{};

        stream.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (stream.gcount() != sizeof(header))
        {
            con::error("IWI header corrupted");
            return std::nullopt;
        }

        const auto* format = GetFormat27(header.format);
        if (format == nullptr)
            return std::nullopt;

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

        auto currentFileSize = sizeof(iwi27::IwiHeader) + sizeof(IwiVersionHeader);
        const auto mipMapCount = hasMipMaps ? texture->GetMipMapCount() : 1;

        for (auto currentMipLevel = mipMapCount - 1; currentMipLevel >= 0; currentMipLevel--)
        {
            const auto sizeOfMipLevel = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
            currentFileSize += sizeOfMipLevel;

            if (currentMipLevel < static_cast<int>(std::extent_v<decltype(iwi27::IwiHeader::fileSizeForPicmip)>)
                && currentFileSize != header.fileSizeForPicmip[currentMipLevel])
            {
                con::error("Iwi has invalid file size for picmip {}", currentMipLevel);
                return std::nullopt;
            }

            stream.read(reinterpret_cast<char*>(texture->GetBufferForMipLevel(currentMipLevel)), sizeOfMipLevel);
            if (stream.gcount() != sizeOfMipLevel)
            {
                con::error("Unexpected eof of iwi in mip level {}", currentMipLevel);
                return std::nullopt;
            }
        }

        CommonIwiMetaData meta{
            .m_no_picmip = (header.flags & iwi27::IwiFlags::IMG_FLAG_NOPICMIP) != 0,
            .m_streaming = (header.flags & iwi27::IwiFlags::IMG_FLAG_STREAMING) != 0,
            .m_clamp_u = (header.flags & iwi27::IwiFlags::IMG_FLAG_CLAMP_U) != 0,
            .m_clamp_v = (header.flags & iwi27::IwiFlags::IMG_FLAG_CLAMP_V) != 0,
            .m_dynamic = (header.flags & iwi27::IwiFlags::IMG_FLAG_DYNAMIC) != 0,
            .m_gamma = header.gamma,
        };

        return IwiLoaderResult{
            .m_version = IwiVersion::IWI_27,
            .m_meta = meta,
            .m_texture = std::move(texture),
        };
    }
} // namespace

namespace image
{
    std::optional<IwiLoaderResult> LoadIwi(std::istream& stream)
    {
        IwiVersionHeader iwiVersionHeader{};

        stream.read(reinterpret_cast<char*>(&iwiVersionHeader), sizeof(iwiVersionHeader));
        if (stream.gcount() != sizeof(iwiVersionHeader))
        {
            con::error("IWI version header corrupted");
            return std::nullopt;
        }

        if (iwiVersionHeader.tag[0] != 'I' || iwiVersionHeader.tag[1] != 'W' || iwiVersionHeader.tag[2] != 'i')
        {
            con::error("Invalid IWI magic");
            return std::nullopt;
        }

        switch (iwiVersionHeader.version)
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

        con::error("Unknown IWI version {}", iwiVersionHeader.version);
        return std::nullopt;
    }
} // namespace image
