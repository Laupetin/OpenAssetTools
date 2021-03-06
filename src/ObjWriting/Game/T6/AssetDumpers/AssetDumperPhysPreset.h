#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Utils/InfoString.h"

namespace T6
{
    class AssetDumperPhysPreset final : public AbstractAssetDumper<PhysPreset>
    {
        static constexpr const char* FILE_TYPE_STR = "PHYSIC";
        static constexpr const char* GDF_NAME = "physpreset.gdf";
        static cspField_t physpreset_fields[];

        static void CopyToPhysPresetInfo(const PhysPreset* physPreset, PhysPresetInfo* physPresetInfo);
        static InfoString CreateInfoString(XAssetInfo<PhysPreset>* asset);

    protected:
        bool ShouldDump(XAssetInfo<PhysPreset>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<PhysPreset>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<PhysPreset>* asset, std::ostream& stream) override;
    };
}
