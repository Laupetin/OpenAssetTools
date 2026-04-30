#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace light_def
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetLightDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetLightDef::Type>& asset) override;
    };
} // namespace light_def
