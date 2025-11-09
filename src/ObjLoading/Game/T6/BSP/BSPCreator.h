#pragma once

#include "BSP.h"
#include "SearchPath/ISearchPath.h"

namespace BSP
{
    std::unique_ptr<BSPData> createBSPData(const std::string& mapName, ISearchPath& searchPath);
}; // namespace BSP
