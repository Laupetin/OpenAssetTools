#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/IW4/IW4.h"

#include <ostream>

namespace IW4
{
    void DumpLeaderboardDefAsJson(std::ostream& stream, const LeaderboardDef* leaderboardDef);
} // namespace IW4
