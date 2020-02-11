#include "IwiWriter27.h"
#include <cassert>

IwiWriter27::IwiWriter27()
= default;

IwiWriter27::~IwiWriter27()
= default;

iwi27::IwiFormat IwiWriter27::GetIwiFormatForImageFormat(const ImageFormat* imageFormat)
{
    switch (imageFormat->GetId())
    {
    case ImageFormatId::R8_G8_B8:
        return iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGB;

    case ImageFormatId::R8_G8_B8_A8:
        return iwi27::IwiFormat::IMG_FORMAT_BITMAP_RGBA;

    case ImageFormatId::A8:
        return iwi27::IwiFormat::IMG_FORMAT_BITMAP_ALPHA;

    case ImageFormatId::R16_G16_B16_A16_FLOAT:
        return iwi27::IwiFormat::IMG_FORMAT_A16B16G16R16F;

    case ImageFormatId::BC1:
        return iwi27::IwiFormat::IMG_FORMAT_DXT1;

    case ImageFormatId::BC2:
        return iwi27::IwiFormat::IMG_FORMAT_DXT3;

    case ImageFormatId::BC3:
        return iwi27::IwiFormat::IMG_FORMAT_DXT5;

    case ImageFormatId::BC5:
        return iwi27::IwiFormat::IMG_FORMAT_DXN;

    default:
        return iwi27::IwiFormat::IMG_FORMAT_INVALID;
    }
}

bool IwiWriter27::SupportsImageFormat(const ImageFormat* imageFormat)
{
    return GetIwiFormatForImageFormat(imageFormat) != iwi27::IwiFormat::IMG_FORMAT_INVALID;
}

std::string IwiWriter27::GetFileExtension()
{
    return ".iwi";
}

void IwiWriter27::WriteVersion(FileAPI::IFile* file)
{
    IwiVersion version{};
    version.tag[0] = 'I';
    version.tag[1] = 'W';
    version.tag[2] = 'i';
    version.version = 27;

    file->Write(&version, sizeof IwiVersion, 1);
}

void IwiWriter27::FillHeader2D(iwi27::IwiHeader* header, Texture2D* texture)
{
    header->dimensions[0] = texture->GetWidth();
    header->dimensions[1] = texture->GetHeight();
    header->dimensions[2] = 1;
}

void IwiWriter27::FillHeaderCube(iwi27::IwiHeader* header, TextureCube* texture)
{
    header->dimensions[0] = texture->GetWidth();
    header->dimensions[1] = texture->GetHeight();
    header->dimensions[2] = 1;
    header->flags |= iwi27::IwiFlags::IMG_FLAG_CUBEMAP;
}

void IwiWriter27::FillHeader3D(iwi27::IwiHeader* header, Texture3D* texture)
{
    header->dimensions[0] = texture->GetWidth();
    header->dimensions[1] = texture->GetHeight();
    header->dimensions[2] = texture->GetDepth();
    header->flags |= iwi27::IwiFlags::IMG_FLAG_VOLMAP;
}

void IwiWriter27::DumpImage(FileAPI::IFile* file, Texture* texture)
{
    assert(file != nullptr);
    assert(texture != nullptr);

    WriteVersion(file);

    iwi27::IwiHeader header{};
    header.flags = 0;
    header.gamma = 0.0f;

    header.format = static_cast<int8_t>(GetIwiFormatForImageFormat(texture->GetFormat()));

    if (!texture->HasMipMaps())
        header.flags |= iwi27::IwiFlags::IMG_FLAG_NOMIPMAPS;

    for (signed char& i : header.maxGlossForMip)
        i = 0;

    size_t currentFileSize = sizeof IwiVersion + sizeof iwi27::IwiHeader;

    const int textureMipCount = texture->HasMipMaps() ? texture->GetMipMapCount() : 1;
    for (int currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const size_t mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel);
        currentFileSize += mipLevelSize;

        if(currentMipLevel < static_cast<int>(_countof(iwi27::IwiHeader::fileSizeForPicmip)))
            header.fileSizeForPicmip[currentMipLevel] = currentFileSize;
    }

    if(auto* texture2D = dynamic_cast<Texture2D*>(texture))
    {
        FillHeader2D(&header, texture2D);
    }
    else if(auto* textureCube = dynamic_cast<TextureCube*>(texture))
    {
        FillHeaderCube(&header, textureCube);
    }
    else if(auto* texture3D = dynamic_cast<Texture3D*>(texture))
    {
        FillHeader3D(&header, texture3D);
    }

    file->Write(&header, sizeof iwi27::IwiHeader, 1);

    for (int currentMipLevel = textureMipCount - 1; currentMipLevel >= 0; currentMipLevel--)
    {
        const size_t mipLevelSize = texture->GetSizeOfMipLevel(currentMipLevel);
        file->Write(texture->GetBufferForMipLevel(currentMipLevel), 1, mipLevelSize);
    }
}
