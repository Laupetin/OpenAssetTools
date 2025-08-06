#include "WeaponCommon.h"

#include <format>

namespace weapon
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("weapons/{}", assetName);
    }
} // namespace weapon
