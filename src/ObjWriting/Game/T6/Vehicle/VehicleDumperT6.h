#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6::vehicle
{
    class Dumper final : public AbstractAssetDumper<VehicleDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset) override;
    };
} // namespace T6::vehicle
