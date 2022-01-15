#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperStructuredDataDefSet final : public AbstractAssetDumper<StructuredDataDefSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<StructuredDataDefSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<StructuredDataDefSet>* asset) override;
    };
}
