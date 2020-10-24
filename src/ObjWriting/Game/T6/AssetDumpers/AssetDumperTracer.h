#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperTracer final : public AbstractAssetDumper<TracerDef>
    {
        static cspField_t tracer_fields[];

    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<TracerDef>* asset, FileAPI::File* out) override;
    };
}
