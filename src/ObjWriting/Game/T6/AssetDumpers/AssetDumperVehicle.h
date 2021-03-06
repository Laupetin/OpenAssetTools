#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Utils/InfoString.h"

namespace T6
{
    class AssetDumperVehicle final : public AbstractAssetDumper<VehicleDef>
    {
        static constexpr const char* FILE_TYPE_STR = "VEHICLEFILE";
        static constexpr const char* GDF_NAME = "vehicle.gdf";
        static cspField_t vehicle_fields[];

        static InfoString CreateInfoString(XAssetInfo<VehicleDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset, std::ostream& stream) override;
    };
}
