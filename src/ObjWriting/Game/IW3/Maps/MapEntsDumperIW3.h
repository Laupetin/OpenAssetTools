#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace map_ents
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::MapEnts>& asset) override;
    };
} // namespace map_ents
