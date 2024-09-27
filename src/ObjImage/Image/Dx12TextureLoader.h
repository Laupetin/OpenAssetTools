#pragma once

#include "Image/DxgiFormat.h"
#include "Image/Texture.h"
#include "Utils/ClassUtils.h"
#include "Utils/MemoryManager.h"

#include <memory>
#include <unordered_map>

class Dx12TextureLoader
{
public:
    Dx12TextureLoader();

    Dx12TextureLoader& Format(oat::DXGI_FORMAT format);
    Dx12TextureLoader& Type(TextureType textureType);
    Dx12TextureLoader& HasMipMaps(bool hasMipMaps);
    Dx12TextureLoader& Width(size_t width);
    Dx12TextureLoader& Height(size_t height);
    Dx12TextureLoader& Depth(size_t depth);

    std::unique_ptr<Texture> LoadTexture(const void* data);

private:
    _NODISCARD const ImageFormat* GetFormatForDx12Format() const;

    static std::unordered_map<ImageFormatId, ImageFormatId> m_conversion_table;

    oat::DXGI_FORMAT m_format;
    TextureType m_type;
    bool m_has_mip_maps;
    size_t m_width;
    size_t m_height;
    size_t m_depth;
};
