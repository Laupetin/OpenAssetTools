#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperRawFile final : public AbstractFileDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream) override;
    };
}
