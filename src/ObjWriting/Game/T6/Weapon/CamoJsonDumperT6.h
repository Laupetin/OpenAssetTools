#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace camo
{
    class JsonDumperT6 final : public AbstractAssetDumper<T6::AssetWeaponCamo>
    {
    public:
        explicit JsonDumperT6(const AssetPool<T6::AssetWeaponCamo::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetWeaponCamo::Type>& asset) override;
    };
} // namespace camo
