#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace leaderboard
{
    class JsonDumperIW4 final : public AbstractAssetDumper<IW4::AssetLeaderboard>
    {
    public:
        explicit JsonDumperIW4(const AssetPool<IW4::AssetLeaderboard::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetLeaderboard::Type>& asset) override;
    };
} // namespace leaderboard
