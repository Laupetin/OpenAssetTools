#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace fx_effect_def
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetFx::Type>& asset) override;
    };
} // namespace fx_effect_def
