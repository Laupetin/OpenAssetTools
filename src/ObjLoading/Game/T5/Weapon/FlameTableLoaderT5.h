#pragma once

#include "Asset/IAssetCreator.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"
#include "Zone/Zone.h"

#include <memory>

namespace weapon
{
    std::unique_ptr<ISubAssetCreator> CreateFlameTableLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace weapon
