#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::weapon
{
    class Dumper final : public AbstractAssetDumper<WeaponVariantDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponVariantDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponVariantDef>* asset) override;
    };
} // namespace T6::weapon
