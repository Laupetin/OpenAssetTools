#include "ImageCommon.h"

#include <algorithm>
#include <format>

namespace image
{
    std::string GetFileNameForAsset(std::string assetName, const std::string& extension)
    {
        std::string cleanAssetName(std::move(assetName));
        std::ranges::replace(cleanAssetName, '*', '_');

        return std::format("images/{}{}", cleanAssetName, extension);
    }
} // namespace image
