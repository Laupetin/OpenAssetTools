#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace leaderboard
{
    class JsonDumperT6 final : public AbstractAssetDumper<T6::LeaderboardDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::LeaderboardDef>& asset) override;
    };
} // namespace leaderboard
