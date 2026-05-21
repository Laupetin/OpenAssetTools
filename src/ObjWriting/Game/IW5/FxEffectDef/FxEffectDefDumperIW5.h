#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace fx_effect_def
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetFx::Type>& asset) override;
    };
} // namespace fx_effect_def
