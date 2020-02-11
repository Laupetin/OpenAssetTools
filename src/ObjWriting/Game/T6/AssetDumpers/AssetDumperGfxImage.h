#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperGfxImage final : public AbstractAssetDumper<T6::GfxImage>
{
protected:
    bool ShouldDump(T6::GfxImage* asset) override;
    std::string GetFileNameForAsset(Zone* zone, T6::GfxImage* asset) override;
    void DumpAsset(Zone* zone, T6::GfxImage* asset, FileAPI::File* out) override;
};