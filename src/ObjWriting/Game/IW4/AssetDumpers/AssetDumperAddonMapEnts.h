#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperAddonMapEnts final : public AbstractFileDumper<AddonMapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<AddonMapEnts>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<AddonMapEnts>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset, std::ostream& stream) override;
    };
}
