#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperStringTable final : public AbstractFileDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset, std::ostream& stream) override;
    };
}
