#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4::vehicle
{
    class Dumper final : public AbstractAssetDumper<VehicleDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<VehicleDef>* asset) override;
    };
} // namespace IW4::vehicle
