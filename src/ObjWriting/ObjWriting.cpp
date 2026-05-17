#include "ObjWriting.h"

#include <type_traits>

namespace
{
    const char* IMAGE_OUTPUT_FORMAT_NAME[]{
        "DDS",
        "IWI",
    };
    static_assert(std::extent_v<decltype(IMAGE_OUTPUT_FORMAT_NAME)> == static_cast<unsigned>(ImageOutputFormat_e::COUNT));
} // namespace

ObjWriting::Configuration_t ObjWriting::Configuration;

const char* GetImageOutputFormatName(ImageOutputFormat_e imageOutputFormat)
{
    if (imageOutputFormat < ImageOutputFormat_e::COUNT)
        return IMAGE_OUTPUT_FORMAT_NAME[static_cast<unsigned>(imageOutputFormat)];

    return "unknown";
}

bool ObjWriting::ShouldHandleAssetType(const asset_type_t assetType)
{
    if (assetType < 0)
        return false;
    if (static_cast<size_t>(assetType) >= Configuration.AssetTypesToHandleBitfield.size())
        return true;

    return Configuration.AssetTypesToHandleBitfield[assetType];
}
