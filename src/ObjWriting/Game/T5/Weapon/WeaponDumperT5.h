#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace weapon
{
    class DumperT5 final : public AbstractAssetDumper<T5::AssetWeapon>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T5::AssetWeapon::Type>& asset) override;
    };
} // namespace weapon
