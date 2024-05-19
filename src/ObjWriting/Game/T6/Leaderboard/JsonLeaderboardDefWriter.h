#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/T6/T6.h"

#include <ostream>

namespace T6
{
    void DumpLeaderboardDefAsJson(std::ostream& stream, const LeaderboardDef* leaderboardDef);
} // namespace T6
