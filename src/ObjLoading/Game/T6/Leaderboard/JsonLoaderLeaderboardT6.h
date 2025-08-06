#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/T6/T6.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace leaderboard
{
    std::unique_ptr<AssetCreator<T6::AssetLeaderboard>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath);
} // namespace leaderboard
