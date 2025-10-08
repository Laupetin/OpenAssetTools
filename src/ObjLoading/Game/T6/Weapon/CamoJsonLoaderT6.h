#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace camo
{
    std::unique_ptr<AssetCreator<T6::AssetWeaponCamo>> CreateJsonLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace camo
