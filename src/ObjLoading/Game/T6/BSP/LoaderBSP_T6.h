#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace BSP
{
    std::unique_ptr<AssetCreator<T6::AssetGfxWorld>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);
} // namespace BSP
