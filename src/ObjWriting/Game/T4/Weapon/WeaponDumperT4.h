#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T4/T4.h"

namespace weapon
{
    class DumperT4 final : public AbstractAssetDumper<T4::AssetWeapon>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T4::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
