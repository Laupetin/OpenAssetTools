#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

namespace T6
{
    class AssetDumperWeapon final : public AbstractAssetDumper<WeaponVariantDef>
    {
        static void CopyToFullDef(const WeaponVariantDef* weapon, WeaponFullDef* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponVariantDef>* asset);
        static void DumpAccuracyGraphs(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponVariantDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset) override;
    };
} // namespace T6
