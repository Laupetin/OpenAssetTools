#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace weapon
{
    std::unique_ptr<AssetCreator<IW5::AssetWeapon>> CreateRawLoaderIW5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace weapon
