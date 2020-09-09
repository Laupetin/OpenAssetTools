#pragma once

#include "IImageWriter.h"
#include "Image/IwiTypes.h"

namespace iwi8
{
    class IwiWriter final : public IImageWriter
    {
        static IwiFormat GetIwiFormatForImageFormat(const ImageFormat* imageFormat);

        static void WriteVersion(FileAPI::IFile* file);
        static void FillHeader2D(IwiHeader* header, Texture2D* texture);
        static void FillHeaderCube(IwiHeader* header, TextureCube* texture);
        static void FillHeader3D(IwiHeader* header, Texture3D* texture);

    public:
        IwiWriter();
        IwiWriter(const IwiWriter& other) = delete;
        IwiWriter(IwiWriter&& other) noexcept = delete;
        ~IwiWriter() override;

        IwiWriter& operator=(const IwiWriter& other) = delete;
        IwiWriter& operator=(IwiWriter&& other) noexcept = delete;

        bool SupportsImageFormat(const ImageFormat* imageFormat) override;
        std::string GetFileExtension() override;
        void DumpImage(FileAPI::IFile* file, Texture* texture) override;
    };
}
