#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "Gdt/IGdtQueryable.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW5::AssetPhysPreset>> CreateGdtLoaderIW5(MemoryManager& memory, IGdtQueryable& gdt, Zone& zone);
} // namespace phys_preset
