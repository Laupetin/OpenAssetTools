#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace addon_map_ents
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AddonMapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::AddonMapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::AddonMapEnts>* asset) override;
    };
} // namespace addon_map_ents
