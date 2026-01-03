#pragma once

#include "Image/D3DFormat.h"
#include "Image/Texture.h"

#include <memory>

namespace image
{
    class Dx9TextureLoader
    {
    public:
        Dx9TextureLoader();

        Dx9TextureLoader& Format(oat::D3DFORMAT format);
        Dx9TextureLoader& Type(TextureType textureType);
        Dx9TextureLoader& HasMipMaps(bool hasMipMaps);
        Dx9TextureLoader& Width(unsigned width);
        Dx9TextureLoader& Height(unsigned height);
        Dx9TextureLoader& Depth(unsigned depth);

        std::unique_ptr<Texture> LoadTexture(const void* data);

    private:
        [[nodiscard]] const ImageFormat* GetFormatForDx9Format() const;

        oat::D3DFORMAT m_format;
        TextureType m_type;
        bool m_has_mip_maps;
        unsigned m_width;
        unsigned m_height;
        unsigned m_depth;
    };
} // namespace image
