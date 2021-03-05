#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperVehicle final : public AbstractFileDumper<VehicleDef>
    {
        static cspField_t vehicle_fields[];
        static cspField_t vehicle_fields2[];

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset, std::ostream& stream) override;
    };
}
