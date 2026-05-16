#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace weapon
{
    std::unique_ptr<AssetCreator<T5::AssetWeapon>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace weapon
