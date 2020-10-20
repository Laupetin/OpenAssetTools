#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperQdb final : public AbstractAssetDumper<Qdb>
    {
    protected:
        bool ShouldDump(XAssetInfo<Qdb>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<Qdb>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<Qdb>* asset, FileAPI::File* out) override;
    };
}
