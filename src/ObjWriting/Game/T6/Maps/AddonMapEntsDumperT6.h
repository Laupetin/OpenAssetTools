#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace addon_map_ents
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetAddonMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetAddonMapEnts::Type>& asset) override;
    };
} // namespace addon_map_ents
