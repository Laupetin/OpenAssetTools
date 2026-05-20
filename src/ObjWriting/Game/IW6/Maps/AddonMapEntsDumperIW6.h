#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace addon_map_ents
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetAddonMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetAddonMapEnts::Type>& asset) override;
    };
} // namespace addon_map_ents
