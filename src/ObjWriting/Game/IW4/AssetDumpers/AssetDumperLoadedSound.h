#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperLoadedSound final : public AbstractAssetDumper<LoadedSound>
    {
        static void DumpWavPcm(Zone* zone, LoadedSound* asset, FileAPI::File* out);
    protected:
        bool ShouldDump(LoadedSound* asset) override;
        std::string GetFileNameForAsset(Zone* zone, LoadedSound* asset) override;
        void DumpAsset(Zone* zone, LoadedSound* asset, FileAPI::File* out) override;
    };
}
