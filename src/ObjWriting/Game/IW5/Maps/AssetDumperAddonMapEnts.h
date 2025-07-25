#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class AssetDumperAddonMapEnts final : public AbstractAssetDumper<AddonMapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<AddonMapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset) override;
    };
} // namespace IW5
