#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace addon_map_ents
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetAddonMapEnts>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetAddonMapEnts::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetAddonMapEnts::Type>& asset) override;
    };
} // namespace addon_map_ents
