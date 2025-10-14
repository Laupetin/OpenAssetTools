#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace shader
{
    class PixelShaderDumperIW4 final : public AbstractAssetDumper<IW4::MaterialPixelShader>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::MaterialPixelShader>& asset) override;
    };
} // namespace shader
