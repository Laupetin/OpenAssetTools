#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace light_def
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetLightDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetLightDef::Type>& asset) override;
    };
} // namespace light_def
