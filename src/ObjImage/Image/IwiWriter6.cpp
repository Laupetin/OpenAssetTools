#include "IwiWriter6.h"

#include <cassert>

using namespace iwi6;

IwiWriter::IwiWriter() = default;

IwiWriter::~IwiWriter() = default;

IwiFormat IwiWriter::GetIwiFormatForImageFormat(const ImageFormat* imageFormat)
{
    switch (imageFormat->GetId())
    {
    case ImageFormatId::R8_G8_B8:
        return IwiFormat::IMG_FORMAT_BITMAP_RGB;

    case ImageFormatId::R8_G8_B8_A8:
        return IwiFormat::IMG_FORMAT_BITMAP_RGBA;

    case ImageFormatId::A8:
        return IwiFormat::IMG_FORMAT_BITMAP_ALPHA;

    case ImageFormatId::BC1:
        return IwiFormat::IMG_FORMAT_DXT1;

    case ImageFormatId::BC2:
        return IwiFormat::IMG_FORMAT_DXT3;

    case ImageFormatId::BC3:
        return IwiFormat::IMG_FORMAT_DXT5;

    case ImageFormatId::BC5:
        return IwiFormat::IMG_FORMAT_DXN;

    default:
        return IwiFormat::IMG_FORMAT_INVALID;
    }
}

void IwiWriter::WriteVersion(std::ostream& stream)
{
    IwiVersion version{};
    version.tag[0] = 'I';
    version.tag[1] = 'W';
    version.tag[2] = 'i';
    version.version = 6;

    stream.write(reinterpret_cast<char*>(&version), sizeof(IwiVersion));
}

void IwiWriter::FillHeader2D(IwiHeader& header, const Texture2D& texture)
{
    header.dimensions[0] = static_cast<uint16_t>(texture.GetWidth());
    header.dimensions[1] = static_cast<uint16_t>(texture.GetHeight());
    header.dimensions[2] = 1u;
}

void IwiWriter::FillHeaderCube(IwiHeader& header, const TextureCube& texture)
{
    header.dimensions[0] = static_cast<uint16_t>(texture.GetWidth());
    header.dimensions[1] = static_cast<uint16_t>(texture.GetHeight());
    header.dimensions[2] = 1u;
    header.flags |= IMG_FLAG_CUBEMAP;
}

void IwiWriter::FillHeader3D(IwiHeader& header, const Texture3D& texture)
{
    header.dimensions[0] = static_cast<uint16_t>(texture.GetWidth());
    header.dimensions[1] = static_cast<uint16_t>(texture.GetHeight());
    header.dimensions[2] = static_cast<uint16_t>(texture.GetDepth());
    header.flags |= IMG_FLAG_VOLMAP;
}

bool IwiWriter::SupportsImageFormat(const ImageFormat* imageFormat)
{
    return GetIwiFormatForImageFormat(imageFormat) != IwiFormat::IMG_FORMAT_INVALID;
}

std::string IwiWriter::GetFileExtension()
{
    return ".iwi";
}

void IwiWriter::DumpImage(std::ostream& stream, const Texture* texture)
{
    assert(texture != nullptr);

    WriteVersion(stream);

    IwiHeader header{};
    header.flags = 0;

    header.format = static_cast<int8_t>(GetIwiFormatForImageFormat(texture->GetFormat()));

    if (!texture->HasMipMaps())
        header.flags |= IMG_FLAG_NOMIPMAPS;

    auto currentFileSize = sizeof(IwiVersion) + sizeof(IwiHeader);

    const auto textureMipCount = texture->HasMipMaps() ? texture->GetMipMapCount() : 1;
    for (auto currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const auto mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        currentFileSize += mipLevelSize;

        if (currentMipLevel < static_cast<int>(std::extent_v<decltype(IwiHeader::fileSizeForPicmip)>))
            header.fileSizeForPicmip[currentMipLevel] = static_cast<uint32_t>(currentFileSize);
    }

    if (const auto* texture2D = dynamic_cast<const Texture2D*>(texture))
    {
        FillHeader2D(header, *texture2D);
    }
    else if (const auto* textureCube = dynamic_cast<const TextureCube*>(texture))
    {
        FillHeaderCube(header, *textureCube);
    }
    else if (const auto* texture3D = dynamic_cast<const Texture3D*>(texture))
    {
        FillHeader3D(header, *texture3D);
    }
    else
    {
        assert(false);
        return;
    }

    stream.write(reinterpret_cast<char*>(&header), sizeof(IwiHeader));

    for (auto currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const auto mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        stream.write(reinterpret_cast<const char*>(texture->GetBufferForMipLevel(currentMipLevel)), mipLevelSize);
    }
}
