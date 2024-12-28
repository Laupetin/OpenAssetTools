#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace IW5
{
    std::unique_ptr<AssetCreator<AssetWeapon>> CreateGdtWeaponLoader(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone);
} // namespace IW5
