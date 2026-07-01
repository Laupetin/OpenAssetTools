#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace weapon
{
    std::unique_ptr<AssetCreator<T4::AssetWeapon>> CreateRawLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace weapon
