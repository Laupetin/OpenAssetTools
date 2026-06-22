#pragma once

#include "ImageDecompressor.h"

namespace image
{
    class DecompressorBc2 final : public ImageDecompressor
    {
    public:
        std::unique_ptr<Texture> Decompress(const Texture& input, const ImageFormat* targetFormat) override;
    };
} // namespace image
