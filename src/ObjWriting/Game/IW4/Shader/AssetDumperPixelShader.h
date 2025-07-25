#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperPixelShader final : public AbstractAssetDumper<MaterialPixelShader>
    {
    protected:
        bool ShouldDump(XAssetInfo<MaterialPixelShader>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialPixelShader>* asset) override;
    };
} // namespace IW4
