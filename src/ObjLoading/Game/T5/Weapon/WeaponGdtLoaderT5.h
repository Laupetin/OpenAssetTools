#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace weapon
{
    std::unique_ptr<AssetCreator<T5::AssetWeapon>> CreateGdtLoaderT5(MemoryManager& memory, ISearchPath& searchPath, IGdtQueryable& gdt, Zone& zone);
} // namespace weapon
