#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperKeyValuePairs final : public AbstractAssetDumper<T6::KeyValuePairs>
{
protected:
    std::string GetFileNameForAsset(T6::KeyValuePairs* asset) override;
    void DumpAsset(T6::KeyValuePairs* asset, FileAPI::File* out) override;
};