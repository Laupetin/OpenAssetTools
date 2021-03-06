#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSlug final : public AbstractAssetDumper<Slug>
    {
    protected:
        bool ShouldDump(XAssetInfo<Slug>* asset) override;
        bool CanDumpAsRaw() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<Slug>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<Slug>* asset, std::ostream& stream) override;
    };
}
