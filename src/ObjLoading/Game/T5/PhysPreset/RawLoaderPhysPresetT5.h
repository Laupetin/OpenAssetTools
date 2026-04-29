#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T5/T5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<T5::AssetPhysPreset>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
