#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperVertexShader final : public AbstractAssetDumper<MaterialVertexShader>
    {
    protected:
        bool ShouldDump(XAssetInfo<MaterialVertexShader>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexShader>* asset) override;
    };
}
