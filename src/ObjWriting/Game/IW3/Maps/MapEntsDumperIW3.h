#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace map_ents
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::MapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW3::MapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW3::MapEnts>* asset) override;
    };
} // namespace map_ents
