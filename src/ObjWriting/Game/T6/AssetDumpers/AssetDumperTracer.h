#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperTracer final : public AbstractFileDumper<TracerDef>
    {
        static cspField_t tracer_fields[];

    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<TracerDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset, std::ostream& stream) override;
    };
}
