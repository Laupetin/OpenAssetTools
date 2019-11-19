#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperStringTable final : public AbstractAssetDumper<T6::StringTable>
{
protected:
    std::string GetFileNameForAsset(Zone* zone, T6::StringTable* asset) override;
    void DumpAsset(Zone* zone, T6::StringTable* asset, FileAPI::File* out) override;
};