#include "AssetDumperLeaderboardDef.h"

#include "Game/IW4/Leaderboard/JsonLeaderboardDefWriter.h"

#include <format>
#include <ranges>

using namespace IW4;

bool AssetDumperLeaderboardDef::ShouldDump(XAssetInfo<LeaderboardDef>* asset)
{
    return true;
}

void AssetDumperLeaderboardDef::DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset)
{
    const auto assetName = asset->m_name;
    const auto assetFile = context.OpenAssetFile(std::format("leaderboards/{}.json", assetName));

    if (!assetFile)
        return;

    DumpLeaderboardDefAsJson(*assetFile, asset->Asset());
}
