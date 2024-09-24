#pragma once

#include "Image/Texture.h"

#include <ostream>
#include <string>

class IImageWriter
{
public:
    IImageWriter() = default;
    virtual ~IImageWriter() = default;
    IImageWriter(const IImageWriter& other) = default;
    IImageWriter(IImageWriter&& other) noexcept = default;
    IImageWriter& operator=(const IImageWriter& other) = default;
    IImageWriter& operator=(IImageWriter&& other) noexcept = default;

    virtual bool SupportsImageFormat(const ImageFormat* imageFormat) = 0;
    virtual std::string GetFileExtension() = 0;
    virtual void DumpImage(std::ostream& stream, const Texture* texture) = 0;
};
