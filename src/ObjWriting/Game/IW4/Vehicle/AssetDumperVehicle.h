#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4
{
    class AssetDumperVehicle final : public AbstractAssetDumper<VehicleDef>
    {
        static InfoString CreateInfoString(XAssetInfo<VehicleDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset) override;
    };
} // namespace IW4
