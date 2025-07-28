#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5::leaderboard
{
    class JsonDumper final : public AbstractAssetDumper<LeaderboardDef>
    {
    protected:
        [[nodiscard]] bool ShouldDump(XAssetInfo<LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset) override;
    };
} // namespace IW5::leaderboard
