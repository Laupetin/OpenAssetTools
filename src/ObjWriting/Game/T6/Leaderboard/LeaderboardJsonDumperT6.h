#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::leaderboard
{
    class JsonDumper final : public AbstractAssetDumper<LeaderboardDef>
    {
    protected:
        [[nodiscard]] bool ShouldDump(XAssetInfo<LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset) override;
    };
} // namespace T6::leaderboard
