#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperSlug final : public AbstractAssetDumper<T6::Slug>
{
protected:
    bool ShouldDump(T6::Slug* asset) override;
    std::string GetFileNameForAsset(Zone* zone, T6::Slug* asset) override;
    void DumpAsset(Zone* zone, T6::Slug* asset, FileAPI::File* out) override;
};