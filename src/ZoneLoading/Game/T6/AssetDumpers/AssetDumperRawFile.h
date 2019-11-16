#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperRawFile final : public AbstractAssetDumper<T6::RawFile>
{
protected:
    std::string GetFileNameForAsset(T6::RawFile* asset) override;
    void DumpAsset(T6::RawFile* asset, FileAPI::File* out) override;
};