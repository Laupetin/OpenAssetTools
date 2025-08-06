#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace z_barrier
{
    class DumperT6 final : public AbstractAssetDumper<T6::ZBarrierDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::ZBarrierDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::ZBarrierDef>* asset) override;
    };
} // namespace z_barrier
