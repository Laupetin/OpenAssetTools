#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace weapon
{
    class WeaponDumperIW3 final : public AbstractAssetDumper<IW3::AssetWeapon>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
