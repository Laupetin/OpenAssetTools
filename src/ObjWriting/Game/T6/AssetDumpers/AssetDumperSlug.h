#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSlug final : public AbstractFileDumper<Slug>
    {
    protected:
        bool ShouldDump(XAssetInfo<Slug>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<Slug>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Slug>* asset, std::ostream& stream) override;
    };
}
