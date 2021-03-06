#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperAddonMapEnts final : public AbstractAssetDumper<AddonMapEnts>
    {
    protected:
        bool ShouldDump(XAssetInfo<AddonMapEnts>* asset) override;
        bool CanDumpAsRaw() override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<AddonMapEnts>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<AddonMapEnts>* asset, std::ostream& stream) override;
    };
}
