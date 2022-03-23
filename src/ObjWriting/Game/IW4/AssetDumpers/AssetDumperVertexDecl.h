#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperVertexDecl final : public AbstractAssetDumper<MaterialVertexDeclaration>
    {
    protected:
        bool ShouldDump(XAssetInfo<MaterialVertexDeclaration>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexDeclaration>* asset) override;
    };
}
