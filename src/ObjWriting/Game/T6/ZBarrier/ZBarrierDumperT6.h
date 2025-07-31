#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::z_barrier
{
    class Dumper final : public AbstractAssetDumper<ZBarrierDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<ZBarrierDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ZBarrierDef>* asset) override;
    };
} // namespace T6::z_barrier
