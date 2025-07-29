#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::addon_map_ents
{
    class Dumper final : public AbstractAssetDumper<AddonMapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<AddonMapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset) override;
    };
} // namespace IW4::addon_map_ents
