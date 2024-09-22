#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

#include <string>

namespace IW5
{
    class AssetDumperMaterial final : public AbstractAssetDumper<Material>
    {
        static std::string GetFileNameForAsset(const std::string& assetName);

    protected:
        bool ShouldDump(XAssetInfo<Material>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset) override;

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<Material>* pool) override;
    };
} // namespace IW5
