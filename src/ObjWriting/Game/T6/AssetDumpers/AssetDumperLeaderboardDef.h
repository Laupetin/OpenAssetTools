#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperLeaderboardDef final : public AbstractAssetDumper<LeaderboardDef>
    {
        static std::string GetFileNameForAsset(const std::string& assetName);

    protected:
        _NODISCARD bool ShouldDump(XAssetInfo<LeaderboardDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LeaderboardDef>* asset) override;
    };
} // namespace T6
