#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace addon_map_ents
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AddonMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AddonMapEnts>& asset) override;
    };
} // namespace addon_map_ents
