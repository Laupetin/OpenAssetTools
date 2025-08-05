#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace camo
{
    class JsonDumperT6 final : public AbstractAssetDumper<T6::WeaponCamo>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::WeaponCamo>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::WeaponCamo>* asset) override;
    };
} // namespace camo
