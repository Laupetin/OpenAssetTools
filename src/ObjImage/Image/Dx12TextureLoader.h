#pragma once

#include "Image/DxgiFormat.h"
#include "Image/Texture.h"

#include <memory>
#include <unordered_map>

namespace image
{
    class Dx12TextureLoader
    {
    public:
        Dx12TextureLoader();

        Dx12TextureLoader& Format(oat::DXGI_FORMAT format);
        Dx12TextureLoader& Type(TextureType textureType);
        Dx12TextureLoader& HasMipMaps(bool hasMipMaps);
        Dx12TextureLoader& Width(unsigned width);
        Dx12TextureLoader& Height(unsigned height);
        Dx12TextureLoader& Depth(unsigned depth);

        std::unique_ptr<Texture> LoadTexture(const void* data);

    private:
        [[nodiscard]] const ImageFormat* GetFormatForDx12Format() const;

        static std::unordered_map<ImageFormatId, ImageFormatId> m_conversion_table;

        oat::DXGI_FORMAT m_format;
        TextureType m_type;
        bool m_has_mip_maps;
        unsigned m_width;
        unsigned m_height;
        unsigned m_depth;
    };
} // namespace image
