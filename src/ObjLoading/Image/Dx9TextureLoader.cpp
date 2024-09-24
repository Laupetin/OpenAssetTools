#include "Dx9TextureLoader.h"

#include <cstring>

Dx9TextureLoader::Dx9TextureLoader()
    : m_format(oat::D3DFMT_UNKNOWN),
      m_type(TextureType::T_2D),
      m_has_mip_maps(false),
      m_width(1u),
      m_height(1u),
      m_depth(1u)
{
}

const ImageFormat* Dx9TextureLoader::GetFormatForDx9Format() const
{
    for (const auto* i : ImageFormat::ALL_FORMATS)
    {
        if (i->GetD3DFormat() == m_format)
            return i;
    }

    return nullptr;
}

Dx9TextureLoader& Dx9TextureLoader::Format(const oat::D3DFORMAT format)
{
    m_format = format;
    return *this;
}

Dx9TextureLoader& Dx9TextureLoader::Type(const TextureType textureType)
{
    m_type = textureType;
    return *this;
}

Dx9TextureLoader& Dx9TextureLoader::HasMipMaps(const bool hasMipMaps)
{
    m_has_mip_maps = hasMipMaps;
    return *this;
}

Dx9TextureLoader& Dx9TextureLoader::Width(const size_t width)
{
    m_width = width;
    return *this;
}

Dx9TextureLoader& Dx9TextureLoader::Height(const size_t height)
{
    m_height = height;
    return *this;
}

Dx9TextureLoader& Dx9TextureLoader::Depth(const size_t depth)
{
    m_depth = depth;
    return *this;
}

std::unique_ptr<Texture> Dx9TextureLoader::LoadTexture(const void* data)
{
    const auto* format = GetFormatForDx9Format();

    if (format == nullptr)
        return nullptr;

    std::unique_ptr<Texture> texture;
    switch (m_type)
    {
    case TextureType::T_2D:
        texture = std::make_unique<Texture2D>(format, m_width, m_height, m_has_mip_maps);
        break;

    case TextureType::T_3D:
        texture = std::make_unique<Texture3D>(format, m_width, m_height, m_depth, m_has_mip_maps);
        break;

    case TextureType::T_CUBE:
        texture = std::make_unique<TextureCube>(format, m_width, m_width, m_has_mip_maps);
        break;

    default:
        return nullptr;
    }

    texture->Allocate();
    const auto mipMapCount = m_has_mip_maps ? texture->GetMipMapCount() : 1;
    const auto faceCount = m_type == TextureType::T_CUBE ? 6 : 1;
    const void* currentDataOffset = data;

    for (auto currentMipLevel = 0; currentMipLevel < mipMapCount; currentMipLevel++)
    {
        for (auto currentFace = 0; currentFace < faceCount; currentFace++)
        {
            const auto mipSize = texture->GetSizeOfMipLevel(currentMipLevel);
            memcpy(texture->GetBufferForMipLevel(currentMipLevel, currentFace), currentDataOffset, mipSize);
            currentDataOffset = reinterpret_cast<const void*>(reinterpret_cast<uintptr_t>(currentDataOffset) + mipSize);
        }
    }

    return texture;
}
