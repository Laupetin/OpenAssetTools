#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace shader
{
    class VertexShaderDumperIW4 final : public AbstractAssetDumper<IW4::MaterialVertexShader>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::MaterialVertexShader>& asset) override;
    };
} // namespace shader
