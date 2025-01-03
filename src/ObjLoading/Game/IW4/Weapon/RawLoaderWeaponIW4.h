#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW4
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawWeaponLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace IW4
