#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace vehicle
{
    std::unique_ptr<AssetCreator<T6::AssetVehicle>> CreateRawLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace vehicle
