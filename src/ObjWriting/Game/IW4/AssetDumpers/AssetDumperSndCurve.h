#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperSndCurve final : public AbstractAssetDumper<SndCurve>
    {
        static std::string GetAssetFilename(const std::string& assetName);

    protected:
        bool ShouldDump(XAssetInfo<SndCurve>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<SndCurve>* asset) override;
    };
}
