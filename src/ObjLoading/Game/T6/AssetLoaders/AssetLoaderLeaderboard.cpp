#include "AssetLoaderLeaderboard.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Leaderboard/JsonLeaderboardDefLoader.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

void* AssetLoaderLeaderboard::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetLeaderboard::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

bool AssetLoaderLeaderboard::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderLeaderboard::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(std::format("leaderboards/{}.json", assetName));
    if (!file.IsOpen())
        return false;

    auto* leaderboardDef = memory->Alloc<LeaderboardDef>();
    leaderboardDef->name = memory->Dup(assetName.c_str());

    if (LoadLeaderboardAsJson(*file.m_stream, *leaderboardDef, memory))
        manager->AddAsset<AssetLeaderboard>(assetName, leaderboardDef);
    else
        std::cerr << std::format("Failed to load leaderboard \"{}\"\n", assetName);

    return true;
}
