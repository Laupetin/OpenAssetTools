#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperStringTable final : public AbstractFileDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<StringTable>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StringTable>* asset, std::ostream& stream) override;
    };
}
