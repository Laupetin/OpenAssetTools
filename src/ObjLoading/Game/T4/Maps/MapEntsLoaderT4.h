#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T4/T4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace map_ents
{
    std::unique_ptr<AssetCreator<T4::AssetMapEnts>> CreateLoaderT4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace map_ents
