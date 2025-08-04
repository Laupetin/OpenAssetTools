#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::sound_curve
{
    class Dumper final : public AbstractAssetDumper<SndCurve>
    {
    protected:
        bool ShouldDump(XAssetInfo<SndCurve>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<SndCurve>* asset) override;
    };
} // namespace IW4::sound_curve
