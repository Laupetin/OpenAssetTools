#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace weapon
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetWeapon>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
