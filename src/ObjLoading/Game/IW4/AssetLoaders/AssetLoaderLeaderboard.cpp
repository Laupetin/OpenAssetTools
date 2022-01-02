#include "AssetLoaderLeaderboard.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderLeaderboard::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* leaderboard = memory->Create<LeaderboardDef>();
    memset(leaderboard, 0, sizeof(LeaderboardDef));
    leaderboard->name = memory->Dup(assetName.c_str());
    return leaderboard;
}
