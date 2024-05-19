#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperLeaderboardDef final : public AbstractAssetDumper<LeaderboardDef>
    {
        static std::string GetFileNameForAsset(const std::string& assetName);

    protected:
        _NODISCARD bool ShouldDump(XAssetInfo<LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset) override;
    };
} // namespace IW4
