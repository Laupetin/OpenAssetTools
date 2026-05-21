#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace fx_impact_table
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetImpactFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetImpactFx::Type>& asset) override;
    };
} // namespace fx_impact_table
