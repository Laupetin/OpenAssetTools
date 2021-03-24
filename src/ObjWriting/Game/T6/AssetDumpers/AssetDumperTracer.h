#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperTracer final : public AbstractAssetDumper<TracerDef>
    {
        static constexpr const char* FILE_TYPE_STR = "TRACER";
        static constexpr const char* GDF_NAME = "tracer.gdf";
        static cspField_t tracer_fields[];

        static InfoString CreateInfoString(XAssetInfo<TracerDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<TracerDef>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<TracerDef>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<TracerDef>* asset, std::ostream& stream) override;
    };
}
