#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW3::AssetPhysPreset>> CreateGdtLoaderIW3(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone);
} // namespace phys_preset
