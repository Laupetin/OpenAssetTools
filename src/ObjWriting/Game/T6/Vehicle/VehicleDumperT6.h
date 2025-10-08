#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace vehicle
{
    class DumperT6 final : public AbstractAssetDumper<T6::VehicleDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::VehicleDef>* asset) override;
    };
} // namespace vehicle
