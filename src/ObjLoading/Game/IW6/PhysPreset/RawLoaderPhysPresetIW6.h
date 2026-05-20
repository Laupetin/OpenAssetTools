#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW6/IW6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW6::AssetPhysPreset>> CreateRawLoaderIW6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
