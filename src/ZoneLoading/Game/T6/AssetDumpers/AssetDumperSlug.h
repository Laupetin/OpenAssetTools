#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperSlug final : public AbstractAssetDumper<T6::Slug>
{
protected:
    std::string GetFileNameForAsset(T6::Slug* asset) override;
    void DumpAsset(T6::Slug* asset, FileAPI::File* out) override;
};