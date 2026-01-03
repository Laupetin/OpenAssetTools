#pragma once

#include "Image/IwiTypes.h"
#include "ImageWriter.h"

namespace image::iwi13
{
    class IwiWriter final : public ImageWriter
    {
        static IwiFormat GetIwiFormatForImageFormat(const ImageFormat* imageFormat);

        static void WriteVersion(std::ostream& stream);
        static void FillHeader2D(IwiHeader& header, const Texture2D& texture);
        static void FillHeaderCube(IwiHeader& header, const TextureCube& texture);
        static void FillHeader3D(IwiHeader& header, const Texture3D& texture);

    public:
        IwiWriter();
        IwiWriter(const IwiWriter& other) = delete;
        IwiWriter(IwiWriter&& other) noexcept = delete;
        ~IwiWriter() override;

        IwiWriter& operator=(const IwiWriter& other) = delete;
        IwiWriter& operator=(IwiWriter&& other) noexcept = delete;

        bool SupportsImageFormat(const ImageFormat* imageFormat) override;
        std::string GetFileExtension() override;
        void DumpImage(std::ostream& stream, const Texture* texture) override;
    };
} // namespace image::iwi13
