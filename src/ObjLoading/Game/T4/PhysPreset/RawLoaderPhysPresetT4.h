#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<T4::AssetPhysPreset>> CreateRawLoaderT4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
