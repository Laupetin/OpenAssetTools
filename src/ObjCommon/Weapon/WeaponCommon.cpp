#include "WeaponCommon.h"

#include <format>

namespace weapon
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("weapons/{}", assetName);
    }

    std::string GetFileNameForFlameTable(const std::string& flameTableName)
    {
        return std::format("weapons/{}", flameTableName);
    }
} // namespace weapon
