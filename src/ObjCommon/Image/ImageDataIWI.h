#pragma once
#include "IImageData.h"

class ImageDataIWI final : public IImageData
{
public:
    ImageFormat* GetFormat() override;

    size_t GetSizeOfMipLevel(unsigned mipLevel) override;
    uint8_t* GetDataForMipLevel(unsigned mipLevel) override;

    bool HasMipMaps() override;
    int GetMipMapCount() override;
    void GenerateMipMaps() override;
    void DiscardMipMaps() override;
};
