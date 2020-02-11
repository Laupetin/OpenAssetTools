#pragma once

#include "IImageWriter.h"
#include "Image/IwiTypes.h"

class IwiWriter27 final : public IImageWriter
{
    static iwi27::IwiFormat GetIwiFormatForImageFormat(const ImageFormat* imageFormat);

    static void WriteVersion(FileAPI::IFile* file);
    static void FillHeader2D(iwi27::IwiHeader* header, Texture2D* texture);
    static void FillHeaderCube(iwi27::IwiHeader* header, TextureCube* texture);
    static void FillHeader3D(iwi27::IwiHeader* header, Texture3D* texture);

public:
    IwiWriter27();
    IwiWriter27(const IwiWriter27& other) = delete;
    IwiWriter27(IwiWriter27&& other) noexcept = delete;
    ~IwiWriter27() override;

    IwiWriter27& operator=(const IwiWriter27& other) = delete;
    IwiWriter27& operator=(IwiWriter27&& other) noexcept = delete;

    bool SupportsImageFormat(const ImageFormat* imageFormat) override;
    std::string GetFileExtension() override;
    void DumpImage(FileAPI::IFile* file, Texture* texture) override;
};
