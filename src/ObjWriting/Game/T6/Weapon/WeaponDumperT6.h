#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace weapon
{
    class DumperT6 final : public AbstractAssetDumper<T6::WeaponVariantDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::WeaponVariantDef>& asset) override;
    };
} // namespace weapon
