#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperRawFile final : public AbstractAssetDumper<RawFile>
    {
    protected:
        bool ShouldDump(RawFile* asset) override;
        std::string GetFileNameForAsset(Zone* zone, RawFile* asset) override;
        void DumpAsset(Zone* zone, RawFile* asset, FileAPI::File* out) override;
    };
}
