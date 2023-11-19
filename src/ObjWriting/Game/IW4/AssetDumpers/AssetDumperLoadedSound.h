#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperLoadedSound final : public AbstractAssetDumper<LoadedSound>
    {
        static void DumpWavPcm(AssetDumpingContext& context, const LoadedSound* asset, std::ostream& stream);

    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset) override;
    };
} // namespace IW4
