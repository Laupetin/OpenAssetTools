#pragma once

#include "Game/IW4/IW4.h"
#include "Utils/MemoryManager.h"

namespace IW4
{
    bool LoadLeaderboardAsJson(std::istream& stream, LeaderboardDef& leaderboard, MemoryManager* memory);
} // namespace IW4
