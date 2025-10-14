#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace leaderboard
{
    class JsonDumperIW4 final : public AbstractAssetDumper<IW4::LeaderboardDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::LeaderboardDef>& asset) override;
    };
} // namespace leaderboard
