#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace fx_effect_def
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetFx::Type>& asset) override;
    };
} // namespace fx_effect_def
