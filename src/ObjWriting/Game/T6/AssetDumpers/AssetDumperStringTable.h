#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperStringTable final : public AbstractAssetDumper<T6::StringTable>
    {
    protected:
        bool ShouldDump(T6::StringTable* asset) override;
        std::string GetFileNameForAsset(Zone* zone, T6::StringTable* asset) override;
        void DumpAsset(Zone* zone, T6::StringTable* asset, FileAPI::File* out) override;
    };
}
