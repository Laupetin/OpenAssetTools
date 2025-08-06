#include "PhysPresetCommon.h"

#include <format>

namespace phys_preset
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("physic/{}", assetName);
    }
} // namespace phys_preset
