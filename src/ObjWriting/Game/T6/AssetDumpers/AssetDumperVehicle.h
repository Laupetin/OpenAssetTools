#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperVehicle final : public AbstractFileDumper<VehicleDef>
    {
        static cspField_t vehicle_fields[];

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset, std::ostream& stream) override;
    };
}
