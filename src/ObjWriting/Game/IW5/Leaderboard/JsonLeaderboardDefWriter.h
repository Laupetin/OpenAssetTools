#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/IW5/IW5.h"

#include <ostream>

namespace IW5
{
    void DumpLeaderboardDefAsJson(std::ostream& stream, const LeaderboardDef* leaderboardDef);
} // namespace IW5
