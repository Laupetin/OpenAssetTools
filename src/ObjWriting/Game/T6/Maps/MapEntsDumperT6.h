#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::map_ents
{
    class Dumper final : public AbstractAssetDumper<MapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<MapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset) override;
    };
} // namespace T6::map_ents
