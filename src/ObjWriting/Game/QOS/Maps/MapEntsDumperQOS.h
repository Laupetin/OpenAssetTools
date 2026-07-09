#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/QOS/QOS.h"

namespace map_ents
{
    class DumperQOS final : public AbstractAssetDumper<QOS::AssetMapEnts>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<QOS::MapEnts>& asset) override;
    };
} // namespace map_ents
