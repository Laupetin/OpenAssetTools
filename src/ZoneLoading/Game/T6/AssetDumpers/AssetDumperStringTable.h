#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperStringTable final : public AbstractAssetDumper<T6::StringTable>
{
protected:
    std::string GetFileNameForAsset(T6::StringTable* asset) override;
    void DumpAsset(T6::StringTable* asset, FileAPI::File* out) override;
};