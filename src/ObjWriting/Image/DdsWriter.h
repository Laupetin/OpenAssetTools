#pragma once
#include "IImageWriter.h"

class DdsWriter final : public IImageWriter
{
public:
    ~DdsWriter() override;

    bool SupportsImageFormat(const ImageFormat * imageFormat) override;
    std::string GetFileExtension() override;
    void DumpImage(std::ostream& stream, Texture* texture) override;
};
