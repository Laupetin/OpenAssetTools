#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"

namespace sound_curve
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::AssetSoundCurve>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::AssetSoundCurve::Type>& asset) override;
    };
} // namespace sound_curve
