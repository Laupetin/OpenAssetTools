#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace map_ents
{
    class DumperT4 final : public AbstractAssetDumper<T4::AssetMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::MapEnts>& asset) override;
    };
} // namespace map_ents
