#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace vehicle
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::VehicleDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::VehicleDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::VehicleDef>* asset) override;
    };
} // namespace vehicle
