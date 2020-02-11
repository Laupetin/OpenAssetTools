#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperRawFile final : public AbstractAssetDumper<T6::RawFile>
{
protected:
    bool ShouldDump(T6::RawFile* asset) override;
    std::string GetFileNameForAsset(Zone* zone, T6::RawFile* asset) override;
    void DumpAsset(Zone* zone, T6::RawFile* asset, FileAPI::File* out) override;
};