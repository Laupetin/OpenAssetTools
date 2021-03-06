#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "Utils/InfoString.h"

namespace IW4
{
    class AssetDumperWeapon final : public AbstractAssetDumper<WeaponCompleteDef>
    {
        static constexpr const char* FILE_TYPE_STR = "WEAPONFILE";
        static constexpr const char* GDF_NAME = "weapon.gdf";
        static cspField_t weapon_fields[];

        static void CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        bool CanDumpAsRaw() override;
        bool CanDumpAsGdtEntry() override;

        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponCompleteDef>* asset) override;
        GdtEntry DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset, std::ostream& stream) override;
    };
}
