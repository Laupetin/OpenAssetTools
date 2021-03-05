#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperWeapon final : public AbstractFileDumper<WeaponVariantDef>
    {
        static cspField_t weapon_fields[];

        void CopyToFullDef(const WeaponVariantDef* weapon, WeaponFullDef* fullDef) const;

    protected:
        bool ShouldDump(XAssetInfo<WeaponVariantDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset, std::ostream& stream) override;
    };
}
