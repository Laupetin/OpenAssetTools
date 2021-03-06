#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
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
