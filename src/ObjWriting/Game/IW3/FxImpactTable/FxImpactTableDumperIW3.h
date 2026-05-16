#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace fx_impact_table
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetImpactFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetImpactFx::Type>& asset) override;
    };
} // namespace fx_impact_table
