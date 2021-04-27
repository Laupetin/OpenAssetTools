#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5
{
    class AssetDumperStringTable final : public AbstractAssetDumper<StringTable>
    {
    protected:
        bool ShouldDump(XAssetInfo<StringTable>* asset) override;
        bool CanDumpAsRaw() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<StringTable>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<StringTable>* asset, std::ostream& stream) override;
    };
}
