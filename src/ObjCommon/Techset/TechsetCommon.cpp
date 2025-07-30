#include "TechsetCommon.h"

#include <format>

namespace techset
{
    std::string GetFileNameForTechniqueName(const std::string& assetName)
    {
        return std::format("techniques/{}.tech", assetName);
    }

    std::string GetFileNameForTechsetName(const std::string& assetName)
    {
        return std::format("techsets/{}.techset", assetName);
    }
} // namespace phys_constraints
