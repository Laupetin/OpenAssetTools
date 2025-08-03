#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace IW4::weapon
{
    class Dumper final : public AbstractAssetDumper<WeaponCompleteDef>
    {
    protected:
        bool ShouldDump(XAssetInfo<WeaponCompleteDef>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<WeaponCompleteDef>* asset) override;
    };
} // namespace IW4::weapon
