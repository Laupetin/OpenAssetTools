#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace vehicle
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetVehicle>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetVehicle::Type>& asset) override;
    };
} // namespace vehicle
