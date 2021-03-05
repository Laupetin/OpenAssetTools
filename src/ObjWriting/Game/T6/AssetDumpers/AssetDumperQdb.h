#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperQdb final : public AbstractFileDumper<Qdb>
    {
    protected:
        bool ShouldDump(XAssetInfo<Qdb>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<Qdb>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Qdb>* asset, std::ostream& stream) override;
    };
}
