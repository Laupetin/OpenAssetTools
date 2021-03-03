#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperVehicle final : public AbstractAssetDumper<VehicleDef>
    {
        static cspField_t vehicle_fields[];

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<VehicleDef>* asset, std::ostream& stream) override;
    };
}
