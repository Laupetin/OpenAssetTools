#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "InfoString/InfoString.h"

namespace IW5::weapon
{
    class Dumper final : public AbstractAssetDumper<WeaponCompleteDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset) override;
    };
} // namespace IW5::weapon
