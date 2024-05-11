#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace T5
{
    class AssetDumperXModel final : public AbstractAssetDumper<XModel>
    {
    protected:
        bool ShouldDump(XAssetInfo<XModel>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset) override;
    };
} // namespace T5
