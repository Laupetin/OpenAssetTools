#include "LeaderboardCommon.h"

#include <format>

namespace leaderboard
{
    std::string GetJsonFileNameForAsset(const std::string& assetName)
    {
        return std::format("leaderboards/{}.json", assetName);
    }
} // namespace leaderboard
