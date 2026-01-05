#pragma once

#include "Image/Texture.h"

#include <ostream>
#include <string>

namespace image
{
    class ImageWriter
    {
    public:
        ImageWriter() = default;
        virtual ~ImageWriter() = default;
        ImageWriter(const ImageWriter& other) = default;
        ImageWriter(ImageWriter&& other) noexcept = default;
        ImageWriter& operator=(const ImageWriter& other) = default;
        ImageWriter& operator=(ImageWriter&& other) noexcept = default;

        virtual bool SupportsImageFormat(const ImageFormat* imageFormat) = 0;
        virtual std::string GetFileExtension() = 0;
        virtual void DumpImage(std::ostream& stream, const Texture* texture) = 0;
    };
} // namespace image
