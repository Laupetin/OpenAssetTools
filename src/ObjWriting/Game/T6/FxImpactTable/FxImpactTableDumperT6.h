#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace fx_impact_table
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetImpactFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetImpactFx::Type>& asset) override;
    };
} // namespace fx_impact_table
