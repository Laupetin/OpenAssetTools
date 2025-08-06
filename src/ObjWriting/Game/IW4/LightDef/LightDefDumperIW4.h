#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace light_def
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::GfxLightDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::GfxLightDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::GfxLightDef>* asset) override;
    };
} // namespace light_def
