#pragma once

#include "Image/Texture.h"

#include <istream>
#include <memory>

namespace image
{
    enum class IwiWaveletFormat
    {
        RGBA,
        RGB,
        LUMINANCE_ALPHA,
        LUMINANCE,
        ALPHA,
    };

    std::unique_ptr<Texture>
        DecodeIwiWavelet(std::istream& stream, TextureType textureType, unsigned width, unsigned height, bool hasMipMaps, IwiWaveletFormat format);
} // namespace image
