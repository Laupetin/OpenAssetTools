#include "LightDefCommon.h"

#include <format>

namespace light_def
{
    std::string GetFileNameForAsset(const std::string& assetName)
    {
        return std::format("lights/{}", assetName);
    }
} // namespace light_def
