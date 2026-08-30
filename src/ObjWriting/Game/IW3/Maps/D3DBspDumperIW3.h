#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace map_d3dbsp
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetGfxWorld>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::GfxWorld>& asset) override;
    };
} // namespace map_d3dbsp
