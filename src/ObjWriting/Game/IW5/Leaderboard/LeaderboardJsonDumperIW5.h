#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace leaderboard
{
    class JsonDumperIW5 final : public AbstractAssetDumper<IW5::LeaderboardDef>
    {
    protected:
        [[nodiscard]] bool ShouldDump(XAssetInfo<IW5::LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::LeaderboardDef>* asset) override;
    };
} // namespace leaderboard
