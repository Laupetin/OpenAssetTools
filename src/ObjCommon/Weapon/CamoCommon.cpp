#include "WeaponCommon.h"

#include <format>

namespace camo
{
    std::string GetJsonFileNameForAssetName(const std::string& assetName)
    {
        return std::format("camo/{}.json", assetName);
    }
} // namespace camo
