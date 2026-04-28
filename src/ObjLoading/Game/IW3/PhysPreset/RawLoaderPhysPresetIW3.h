#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW3/IW3.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW3::AssetPhysPreset>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
