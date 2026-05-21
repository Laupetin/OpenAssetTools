#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace fx_effect_def
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetFx>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetFx::Type>& asset) override;
    };
} // namespace fx_effect_def
