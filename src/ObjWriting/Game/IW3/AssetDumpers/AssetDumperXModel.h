#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace IW3
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
} // namespace IW3
