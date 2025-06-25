#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperMaterial final : public AbstractAssetDumper<Material>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<Material>* pool) override;

    protected:
        bool ShouldDump(XAssetInfo<Material>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset) override;
    };
} // namespace T6
