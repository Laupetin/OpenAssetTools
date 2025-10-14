#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"
#include "InfoString/InfoString.h"

namespace weapon
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::WeaponCompleteDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::WeaponCompleteDef>& asset) override;
    };
} // namespace weapon
