#include "PhysCollmapCommon.h"

#include <format>

namespace phys_collmap
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("phys_collmaps/{}.map", assetName);
    }
} // namespace phys_collmap
