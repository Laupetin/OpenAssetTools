#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperWeaponCamo final : public AbstractAssetDumper<WeaponCamo>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponCamo>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCamo>* asset) override;
    };
} // namespace T6
