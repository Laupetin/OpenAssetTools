#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperStringTable final : public AbstractAssetDumper<IW4::StringTable>
    {
    protected:
        bool ShouldDump(IW4::StringTable* asset) override;
        std::string GetFileNameForAsset(Zone* zone, IW4::StringTable* asset) override;
        void DumpAsset(Zone* zone, IW4::StringTable* asset, FileAPI::File* out) override;
    };
}
