#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW5::weapon
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateRawLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace IW5::weapon
