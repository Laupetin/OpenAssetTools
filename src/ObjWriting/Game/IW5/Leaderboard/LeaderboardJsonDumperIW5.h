#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace leaderboard
{
    class JsonDumperIW5 final : public AbstractAssetDumper<IW5::AssetLeaderboard>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetLeaderboard::Type>& asset) override;
    };
} // namespace leaderboard
