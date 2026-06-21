#pragma once

#include "ImageWriter.h"

#include <memory>

namespace image
{
    std::unique_ptr<Texture> ConvertTextureForDdsFileOutputIfNecessary(const Texture* texture);

    class DdsWriter final : public ImageWriter
    {
    public:
        bool SupportsImageFormat(const ImageFormat* imageFormat) override;
        std::string GetFileExtension() override;
        void DumpImage(std::ostream& stream, const Texture* texture) override;
    };
} // namespace image
