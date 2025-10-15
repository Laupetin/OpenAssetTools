#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace addon_map_ents
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetAddonMapEnts>
    {
    public:
        explicit DumperIW5(const AssetPool<IW5::AssetAddonMapEnts::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetAddonMapEnts::Type>& asset) override;
    };
} // namespace addon_map_ents
