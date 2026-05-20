#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace leaderboard
{
    class JsonDumperIW6 final : public AbstractAssetDumper<IW6::AssetLeaderboard>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetLeaderboard::Type>& asset) override;
    };
} // namespace leaderboard
