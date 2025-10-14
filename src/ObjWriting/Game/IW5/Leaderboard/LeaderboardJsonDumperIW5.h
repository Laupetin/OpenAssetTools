#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace leaderboard
{
    class JsonDumperIW5 final : public AbstractAssetDumper<IW5::LeaderboardDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::LeaderboardDef>& asset) override;
    };
} // namespace leaderboard
