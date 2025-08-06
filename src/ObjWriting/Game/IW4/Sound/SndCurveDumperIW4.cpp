#include "SndCurveDumperIW4.h"

#include "Sound/SndCurveDumper.h"
#include "Sound/SoundCurveCommon.h"

#include <sstream>

using namespace IW4;

namespace sound_curve
{
    bool DumperIW4::ShouldDump(XAssetInfo<SndCurve>* asset)
    {
        return true;
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, XAssetInfo<SndCurve>* asset)
    {
        const auto* sndCurve = asset->Asset();

        const auto assetFile = context.OpenAssetFile(GetFileNameForAssetName(sndCurve->filename));

        if (!assetFile)
            return;

        SndCurveDumper dumper(*assetFile);

        const auto knotCount = std::min(static_cast<size_t>(sndCurve->knotCount), std::extent_v<decltype(SndCurve::knots)>);
        dumper.Init(knotCount);

        for (auto i = 0u; i < knotCount; i++)
            dumper.WriteKnot(sndCurve->knots[i][0], sndCurve->knots[i][1]);
    }
} // namespace sound_curve
