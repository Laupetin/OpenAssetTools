#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSlug final : public AbstractAssetDumper<Slug>
    {
    protected:
        bool ShouldDump(XAssetInfo<Slug>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<Slug>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<Slug>* asset, FileAPI::File* out) override;
    };
}
