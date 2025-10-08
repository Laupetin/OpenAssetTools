#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW5/IW5.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace leaderboard
{
    std::unique_ptr<AssetCreator<IW5::AssetLeaderboard>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath);
} // namespace leaderboard
