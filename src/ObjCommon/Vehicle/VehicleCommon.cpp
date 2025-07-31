#include "VehicleCommon.h"

#include <format>

namespace vehicle
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("vehicles/{}", assetName);
    }
} // namespace vehicle
