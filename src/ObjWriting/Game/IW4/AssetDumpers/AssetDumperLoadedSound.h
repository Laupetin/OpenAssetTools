#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperLoadedSound final : public AbstractFileDumper<LoadedSound>
    {
        static void DumpWavPcm(AssetDumpingContext& context, const LoadedSound* asset, std::ostream& stream);
    protected:
        bool ShouldDump(XAssetInfo<LoadedSound>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<LoadedSound>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<LoadedSound>* asset, std::ostream& stream) override;
    };
}
