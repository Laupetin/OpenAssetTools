#include "PhysConstraintsCommon.h"

#include <format>

namespace phys_constraints
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("physconstraints/{}", assetName);
    }
} // namespace phys_constraints
