#include "AttachmentCommon.h"

#include <format>

namespace attachment
{
    std::string GetInfoStringFileNameForAssetName(const std::string& assetName)
    {
        return std::format("attachment/{}", assetName);
    }

    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        return std::format("attachment/{}.json", assetName);
    }
} // namespace attachment
