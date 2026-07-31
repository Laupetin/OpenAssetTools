#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<T4::AssetPhysPreset>> CreateGdtLoaderT4(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone);
} // namespace phys_preset
