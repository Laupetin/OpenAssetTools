#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperGfxLightDef final : public AbstractAssetDumper<GfxLightDef>
    {
        static std::string GetAssetFilename(const std::string& assetName);

    protected:
        bool ShouldDump(XAssetInfo<GfxLightDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxLightDef>* asset) override;
    };
} // namespace IW4
