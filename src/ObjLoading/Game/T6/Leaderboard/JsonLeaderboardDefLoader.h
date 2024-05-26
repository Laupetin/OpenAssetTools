#pragma once

#include "Game/T6/T6.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace T6
{
    bool LoadLeaderboardAsJson(std::istream& stream, LeaderboardDef& leaderboard, MemoryManager* memory);
} // namespace T6
