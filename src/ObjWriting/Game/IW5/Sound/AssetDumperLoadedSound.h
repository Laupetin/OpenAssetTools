#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class AssetDumperLoadedSound final : public AbstractAssetDumper<LoadedSound>
    {
        static void DumpWavPcm(const LoadedSound* asset, std::ostream& stream);

    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset) override;
    };
} // namespace IW5
