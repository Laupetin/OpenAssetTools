#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperRawFile final : public AbstractFileDumper<RawFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<RawFile>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream) override;
    };
}
