#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace fx_impact_table
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetImpactFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetImpactFx::Type>& asset) override;
    };
} // namespace fx_impact_table
