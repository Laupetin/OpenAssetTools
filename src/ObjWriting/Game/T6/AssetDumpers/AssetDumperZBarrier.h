#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperZBarrier final : public AbstractAssetDumper<ZBarrierDef>
    {
        static cspField_t zbarrier_fields[];

    protected:
        bool ShouldDump(XAssetInfo<ZBarrierDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<ZBarrierDef>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<ZBarrierDef>* asset, FileAPI::File* out) override;
    };
}
