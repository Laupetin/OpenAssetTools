#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperWeapon final : public AbstractAssetDumper<WeaponVariantDef>
    {
        static const std::string EMPTY_STRING;
        static cspField_t weapon_fields[];

        void CopyToFullDef(const WeaponVariantDef* weapon, WeaponFullDef* fullDef) const;

    protected:
        bool ShouldDump(XAssetInfo<WeaponVariantDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset) override;
        void DumpAsset(Zone* zone, XAssetInfo<WeaponVariantDef>* asset, FileAPI::File* out) override;
    };
}
