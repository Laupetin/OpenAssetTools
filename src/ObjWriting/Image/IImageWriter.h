#pragma once

#include "Image/Texture.h"
#include "Utils/FileAPI.h"
#include <string>

class IImageWriter
{
public:
    virtual ~IImageWriter() = default;

    virtual bool SupportsImageFormat(const ImageFormat* imageFormat) = 0;
    virtual std::string GetFileExtension() = 0;
    virtual void DumpImage(FileAPI::IFile* file, Texture* texture) = 0;
};