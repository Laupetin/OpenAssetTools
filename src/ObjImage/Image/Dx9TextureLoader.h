#pragma once

#include "Image/D3DFormat.h"
#include "Image/Texture.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

#include <memory>
#include <unordered_map>

class Dx9TextureLoader
{
public:
    Dx9TextureLoader();

    Dx9TextureLoader& Format(oat::D3DFORMAT format);
    Dx9TextureLoader& Type(TextureType textureType);
    Dx9TextureLoader& HasMipMaps(bool hasMipMaps);
    Dx9TextureLoader& Width(size_t width);
    Dx9TextureLoader& Height(size_t height);
    Dx9TextureLoader& Depth(size_t depth);

    std::unique_ptr<Texture> LoadTexture(const void* data);

private:
    _NODISCARD const ImageFormat* GetFormatForDx9Format() const;

    oat::D3DFORMAT m_format;
    TextureType m_type;
    bool m_has_mip_maps;
    size_t m_width;
    size_t m_height;
    size_t m_depth;
};
