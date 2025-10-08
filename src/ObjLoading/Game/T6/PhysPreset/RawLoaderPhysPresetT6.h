#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<T6::AssetPhysPreset>> CreateRawLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
