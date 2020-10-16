#include "IwiWriter8.h"

#include <cassert>

using namespace iwi8;

IwiWriter::IwiWriter()
= default;

IwiWriter::~IwiWriter()
= default;

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

void IwiWriter::WriteVersion(FileAPI::IFile* file)
{
    IwiVersion version{};
    version.tag[0] = 'I';
    version.tag[1] = 'W';
    version.tag[2] = 'i';
    version.version = 8;

    file->Write(&version, sizeof IwiVersion, 1);
}

void IwiWriter::FillHeader2D(IwiHeader* header, Texture2D* texture)
{
    header->dimensions[0] = static_cast<uint16_t>(texture->GetWidth());
    header->dimensions[1] = static_cast<uint16_t>(texture->GetHeight());
    header->dimensions[2] = 1ui16;
}

void IwiWriter::FillHeaderCube(IwiHeader* header, TextureCube* texture)
{
    header->dimensions[0] = static_cast<uint16_t>(texture->GetWidth());
    header->dimensions[1] = static_cast<uint16_t>(texture->GetHeight());
    header->dimensions[2] = 1ui16;
    header->flags |= IMG_FLAG_MAPTYPE_CUBE;
}

void IwiWriter::FillHeader3D(IwiHeader* header, Texture3D* texture)
{
    header->dimensions[0] = static_cast<uint16_t>(texture->GetWidth());
    header->dimensions[1] = static_cast<uint16_t>(texture->GetHeight());
    header->dimensions[2] = static_cast<uint16_t>(texture->GetDepth());
    header->flags |= IMG_FLAG_MAPTYPE_3D;
}

bool IwiWriter::SupportsImageFormat(const ImageFormat* imageFormat)
{
    return GetIwiFormatForImageFormat(imageFormat) != IwiFormat::IMG_FORMAT_INVALID;
}

std::string IwiWriter::GetFileExtension()
{
    return ".iwi";
}

void IwiWriter::DumpImage(FileAPI::IFile* file, Texture* texture)
{
    assert(file != nullptr);
    assert(texture != nullptr);

    WriteVersion(file);

    IwiHeader header{};
    header.flags = 0;

    header.format = static_cast<int8_t>(GetIwiFormatForImageFormat(texture->GetFormat()));

    if (!texture->HasMipMaps())
        header.flags |= IMG_FLAG_NOMIPMAPS;

    size_t currentFileSize = sizeof IwiVersion + sizeof IwiHeader;

    const int textureMipCount = texture->HasMipMaps() ? texture->GetMipMapCount() : 1;
    for (int currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const size_t mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        currentFileSize += mipLevelSize;

        if (currentMipLevel < static_cast<int>(_countof(iwi27::IwiHeader::fileSizeForPicmip)))
            header.fileSizeForPicmip[currentMipLevel] = currentFileSize;
    }

    if (auto* texture2D = dynamic_cast<Texture2D*>(texture))
    {
        FillHeader2D(&header, texture2D);
    }
    else if (auto* textureCube = dynamic_cast<TextureCube*>(texture))
    {
        FillHeaderCube(&header, textureCube);
    }
    else if (auto* texture3D = dynamic_cast<Texture3D*>(texture))
    {
        FillHeader3D(&header, texture3D);
    }

    file->Write(&header, sizeof iwi27::IwiHeader, 1);

    for (int currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const size_t mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel) * texture->GetFaceCount();
        file->Write(texture->GetBufferForMipLevel(currentMipLevel), 1, mipLevelSize);
    }
}
