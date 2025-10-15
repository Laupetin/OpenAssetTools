#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace weapon
{
    class DumperT6 final : public AbstractAssetDumper<T6::AssetWeapon>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetWeapon::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
