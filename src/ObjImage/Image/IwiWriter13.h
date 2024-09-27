#pragma once

#include "IImageWriter.h"
#include "Image/IwiTypes.h"

namespace iwi13
{
    class IwiWriter final : public IImageWriter
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
} // namespace iwi13
