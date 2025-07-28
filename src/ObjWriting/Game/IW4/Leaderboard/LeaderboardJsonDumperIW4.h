#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::leaderboard
{
    class JsonDumper final : public AbstractAssetDumper<LeaderboardDef>
    {
    protected:
        [[nodiscard]] bool ShouldDump(XAssetInfo<LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset) override;
    };
} // namespace IW4::leaderboard
