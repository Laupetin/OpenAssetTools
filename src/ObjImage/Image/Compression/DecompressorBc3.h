#pragma once

#include "ImageDecompressor.h"

namespace image
{
    class DecompressorBc3 final : public ImageDecompressor
    {
    public:
        std::unique_ptr<Texture> Decompress(const Texture& input, const ImageFormat* targetFormat) override;
    };
} // namespace image
