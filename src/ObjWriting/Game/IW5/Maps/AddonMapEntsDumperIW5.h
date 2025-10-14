#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace addon_map_ents
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AddonMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AddonMapEnts>& asset) override;
    };
} // namespace addon_map_ents
