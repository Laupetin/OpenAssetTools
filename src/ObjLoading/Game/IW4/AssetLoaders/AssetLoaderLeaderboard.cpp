#include "AssetLoaderLeaderboard.h"

#include "Game/IW4/IW4.h"
#include "Game/IW4/Leaderboard/JsonLeaderboardDefLoader.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace IW4;

void* AssetLoaderLeaderboard::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* leaderboard = memory->Create<LeaderboardDef>();
    memset(leaderboard, 0, sizeof(LeaderboardDef));
    leaderboard->name = memory->Dup(assetName.c_str());
    return leaderboard;
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
