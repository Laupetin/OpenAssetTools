#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperMaterial final : public AbstractAssetDumper<Material>
    {
    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<Material>* pool) override;

    protected:
        bool ShouldDump(XAssetInfo<Material>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset) override;
    };
} // namespace IW4
