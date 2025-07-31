#include "SndCurveDumperIW4.h"

#include "Dumping/SndCurve/SndCurveDumper.h"

#include <sstream>

using namespace IW4;

namespace
{
    std::string GetAssetFilename(const std::string& assetName)
    {
        std::ostringstream ss;

        ss << "soundaliases/" << assetName << ".vfcurve";

        return ss.str();
    }
} // namespace

namespace IW4::sound
{
    bool SndCurveDumper::ShouldDump(XAssetInfo<SndCurve>* asset)
    {
        return true;
    }

    void SndCurveDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<SndCurve>* asset)
    {
        const auto* sndCurve = asset->Asset();

        const auto assetFile = context.OpenAssetFile(GetAssetFilename(sndCurve->filename));

        if (!assetFile)
            return;

        ::SndCurveDumper dumper(*assetFile);

        const auto knotCount = std::min(static_cast<size_t>(sndCurve->knotCount), std::extent_v<decltype(SndCurve::knots)>);
        dumper.Init(knotCount);

        for (auto i = 0u; i < knotCount; i++)
            dumper.WriteKnot(sndCurve->knots[i][0], sndCurve->knots[i][1]);
    }
} // namespace IW4::sound
