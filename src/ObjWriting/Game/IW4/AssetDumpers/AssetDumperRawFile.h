#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperRawFile final : public AbstractAssetDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<RawFile>* asset, std::ostream& stream) override;
    };
}
