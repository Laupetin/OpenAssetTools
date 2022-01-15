#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4
{
    class AssetDumperWeapon final : public AbstractAssetDumper<WeaponCompleteDef>
    {
        static void CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset) override;
    };
}
