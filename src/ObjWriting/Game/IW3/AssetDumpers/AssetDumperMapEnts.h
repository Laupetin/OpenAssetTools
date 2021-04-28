#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace IW3
{
    class AssetDumperMapEnts final : public AbstractAssetDumper<MapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<MapEnts>* asset) override;
        bool CanDumpAsRaw() override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<MapEnts>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<MapEnts>* asset, std::ostream& stream) override;
    };
}
