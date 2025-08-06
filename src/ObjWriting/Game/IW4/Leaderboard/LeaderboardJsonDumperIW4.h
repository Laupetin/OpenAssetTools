#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace leaderboard
{
    class JsonDumperIW4 final : public AbstractAssetDumper<IW4::LeaderboardDef>
    {
    protected:
        [[nodiscard]] bool ShouldDump(XAssetInfo<IW4::LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::LeaderboardDef>* asset) override;
    };
} // namespace leaderboard
