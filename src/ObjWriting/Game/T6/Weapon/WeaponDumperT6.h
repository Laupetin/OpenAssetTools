#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace weapon
{
    class DumperT6 final : public AbstractAssetDumper<T6::WeaponVariantDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::WeaponVariantDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::WeaponVariantDef>* asset) override;
    };
} // namespace weapon
