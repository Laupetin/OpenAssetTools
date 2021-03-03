#pragma once

#include <ostream>
#include <string>

#include "Image/Texture.h"

class IImageWriter
{
public:
    virtual ~IImageWriter() = default;

    virtual bool SupportsImageFormat(const ImageFormat* imageFormat) = 0;
    virtual std::string GetFileExtension() = 0;
    virtual void DumpImage(std::ostream& stream, Texture* texture) = 0;
};