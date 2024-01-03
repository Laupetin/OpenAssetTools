#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

#include <string>

namespace T6
{
    class AssetDumperMaterial final : public AbstractAssetDumper<Material>
    {
        static std::string GetFileNameForAsset(const std::string& assetName);

    protected:
        bool ShouldDump(XAssetInfo<Material>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Material>* asset) override;
    };
} // namespace T6
