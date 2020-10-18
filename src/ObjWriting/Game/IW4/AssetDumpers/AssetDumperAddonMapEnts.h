#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperAddonMapEnts final : public AbstractAssetDumper<AddonMapEnts>
    {
    protected:
        bool ShouldDump(AddonMapEnts* asset) override;
        std::string GetFileNameForAsset(Zone* zone, AddonMapEnts* asset) override;
        void DumpAsset(Zone* zone, AddonMapEnts* asset, FileAPI::File* out) override;
    };
}
