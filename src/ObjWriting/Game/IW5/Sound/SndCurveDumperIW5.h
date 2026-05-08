#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace sound_curve
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::AssetSoundCurve>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW5::AssetSoundCurve::Type>& asset) override;
    };
} // namespace sound_curve
