#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace map_ents
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetMapEnts::Type>& asset) override;
    };
} // namespace map_ents
