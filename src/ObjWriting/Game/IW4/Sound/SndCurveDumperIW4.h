#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace sound_curve
{
    class DumperIW4 final : public AbstractAssetDumper<IW4::SndCurve>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW4::SndCurve>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW4::SndCurve>* asset) override;
    };
} // namespace sound_curve
