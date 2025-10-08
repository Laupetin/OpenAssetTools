#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace phys_preset
{
    std::unique_ptr<AssetCreator<IW4::AssetPhysPreset>> CreateRawLoaderIW4(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace phys_preset
