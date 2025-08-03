#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::light_def
{
    class Dumper final : public AbstractAssetDumper<GfxLightDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<GfxLightDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxLightDef>* asset) override;
    };
} // namespace IW4::light_def
