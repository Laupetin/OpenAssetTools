#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperZBarrier final : public AbstractFileDumper<ZBarrierDef>
    {
        static cspField_t zbarrier_fields[];

    protected:
        bool ShouldDump(XAssetInfo<ZBarrierDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<ZBarrierDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ZBarrierDef>* asset, std::ostream& stream) override;
    };
}
