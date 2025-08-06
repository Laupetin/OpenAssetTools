#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace shader
{
    class VertexShaderDumperIW4 final : public AbstractAssetDumper<IW4::MaterialVertexShader>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::MaterialVertexShader>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::MaterialVertexShader>* asset) override;
    };
} // namespace shader
