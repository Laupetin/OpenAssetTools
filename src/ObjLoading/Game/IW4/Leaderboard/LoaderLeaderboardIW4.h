#pragma once

#include "Asset/IAssetCreator.h"
#include "Game/IW4/IW4.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/MemoryManager.h"

#include <memory>

namespace leaderboard
{
    std::unique_ptr<AssetCreator<IW4::AssetLeaderboard>> CreateLoaderIW4(MemoryManager& memory, ISearchPath& searchPath);
} // namespace leaderboard
