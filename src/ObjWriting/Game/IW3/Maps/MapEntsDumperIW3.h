#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace IW3::map_ents
{
    class Dumper final : public AbstractAssetDumper<MapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<MapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset) override;
    };
} // namespace IW3::map_ents
