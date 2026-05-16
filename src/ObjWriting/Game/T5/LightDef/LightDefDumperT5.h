#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace light_def
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetLightDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetLightDef::Type>& asset) override;
    };
} // namespace light_def
