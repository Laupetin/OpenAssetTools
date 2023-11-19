#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperPhysCollmap final : public AbstractAssetDumper<PhysCollmap>
    {
        static std::string GetAssetFilename(const std::string& assetName);

    protected:
        bool ShouldDump(XAssetInfo<PhysCollmap>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<PhysCollmap>* asset) override;
    };
} // namespace IW4
