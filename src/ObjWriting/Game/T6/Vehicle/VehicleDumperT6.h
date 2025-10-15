#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace vehicle
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetVehicle>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetVehicle::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetVehicle::Type>& asset) override;
    };
} // namespace vehicle
