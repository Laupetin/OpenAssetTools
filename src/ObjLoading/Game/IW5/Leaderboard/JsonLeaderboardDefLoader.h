#pragma once

#include "Game/IW5/IW5.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace IW5
{
    bool LoadLeaderboardAsJson(std::istream& stream, LeaderboardDef& leaderboard, MemoryManager& memory);
} // namespace IW5
