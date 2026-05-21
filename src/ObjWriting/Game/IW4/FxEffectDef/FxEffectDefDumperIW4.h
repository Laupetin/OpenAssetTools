#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace fx_effect_def
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetFx::Type>& asset) override;
    };
} // namespace fx_effect_def
