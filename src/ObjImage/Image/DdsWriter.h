#pragma once

#include "ImageWriter.h"

namespace image
{
    class DdsWriter final : public ImageWriter
    {
    public:
        ~DdsWriter() override;

        bool SupportsImageFormat(const ImageFormat* imageFormat) override;
        std::string GetFileExtension() override;
        void DumpImage(std::ostream& stream, const Texture* texture) override;
    };
} // namespace image
