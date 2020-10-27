#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperVehicle final : public AbstractAssetDumper<VehicleDef>
    {
        static cspField_t vehicle_fields[];
        static cspField_t vehicle_fields2[];

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<VehicleDef>* asset, FileAPI::File* out) override;
    };
}
