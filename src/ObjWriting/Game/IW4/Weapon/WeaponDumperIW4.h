#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace weapon
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::WeaponCompleteDef>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::WeaponCompleteDef>& asset) override;
    };
} // namespace weapon
