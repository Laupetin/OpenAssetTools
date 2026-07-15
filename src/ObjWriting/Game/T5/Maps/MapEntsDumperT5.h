#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace map_ents
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetMapEnts::Type>& asset) override;
    };
} // namespace map_ents
