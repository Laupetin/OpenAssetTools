#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<T5::AssetPhysPreset>> CreateGdtLoaderT5(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone);
} // namespace phys_preset
