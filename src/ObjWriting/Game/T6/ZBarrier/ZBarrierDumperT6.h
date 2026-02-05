#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace z_barrier
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetZBarrier>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetZBarrier::Type>& asset) override;
    };
} // namespace z_barrier
