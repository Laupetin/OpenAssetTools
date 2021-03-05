#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperWeapon final : public AbstractFileDumper<WeaponCompleteDef>
    {
        static cspField_t weapon_fields[];

        static void CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef);

    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset, std::ostream& stream) override;
    };
}
