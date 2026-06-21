#pragma once

#include "Image/Texture.h"

#include <limits>
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

        virtual std::unique_ptr<Texture> Decompress(const Texture& input, const ImageFormat* targetFormat) = 0;

        static ImageDecompressor* GetDecompressorForFormat(ImageFormatId formatId);

    protected:
        static constexpr uint64_t Mask1(const unsigned length)
        {
            if (length >= sizeof(uint64_t) * 8)
                return std::numeric_limits<uint64_t>::max();

            return std::numeric_limits<uint64_t>::max() >> (sizeof(uint64_t) * 8 - length);
        }
    };
} // namespace image
