#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "InfoString/InfoString.h"

namespace IW5
{
    class AssetDumperWeapon final : public AbstractAssetDumper<WeaponCompleteDef>
    {
        static void CopyToFullDef(const WeaponCompleteDef* weapon, WeaponFullDef* fullDef);
        static InfoString CreateInfoString(XAssetInfo<WeaponCompleteDef>* asset);

    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset) override;
    };
} // namespace IW5
