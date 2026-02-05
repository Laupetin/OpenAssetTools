#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace sound_curve
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::AssetSoundCurve>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetSoundCurve::Type>& asset) override;
    };
} // namespace sound_curve
