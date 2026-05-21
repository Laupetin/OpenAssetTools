#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace fx_impact_table
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetImpactFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetImpactFx::Type>& asset) override;
    };
} // namespace fx_impact_table
