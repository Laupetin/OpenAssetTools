#pragma once

#include "Image/Texture.h"

#include <memory>

namespace image
{
    class ImageDecompressor
    {
    public:
        ImageDecompressor() = default;
        virtual ~ImageDecompressor() = default;
        ImageDecompressor(const ImageDecompressor& other) = default;
        ImageDecompressor(ImageDecompressor&& other) noexcept = default;
        ImageDecompressor& operator=(const ImageDecompressor& other) = default;
        ImageDecompressor& operator=(ImageDecompressor&& other) noexcept = default;

        virtual std::unique_ptr<Texture> Decompress(const Texture& input) = 0;

        static ImageDecompressor* GetDecompressorForFormat(ImageFormatId formatId);
    };
} // namespace image
