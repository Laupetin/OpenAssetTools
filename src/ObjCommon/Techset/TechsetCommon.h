#pragma once

#include <string>

namespace techset
{
    std::string GetFileNameForStateMapName(const std::string& stateMapName);
    std::string GetFileNameForTechniqueName(const std::string& assetName);
    std::string GetFileNameForTechsetName(const std::string& assetName);
} // namespace techset
