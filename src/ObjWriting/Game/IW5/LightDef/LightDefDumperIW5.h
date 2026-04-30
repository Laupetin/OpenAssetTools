#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace light_def
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetLightDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetLightDef::Type>& asset) override;
    };
} // namespace light_def
