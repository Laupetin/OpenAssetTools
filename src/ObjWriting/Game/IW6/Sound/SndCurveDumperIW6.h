#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"

namespace sound_curve
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetSoundCurve>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetSoundCurve::Type>& asset) override;
    };
} // namespace sound_curve
