#include "AssetDumperSndCurve.h"

#include <sstream>

#include "Dumping/SndCurve/SndCurveDumper.h"

using namespace IW4;

std::string AssetDumperSndCurve::GetAssetFilename(const std::string& assetName)
{
    std::ostringstream ss;

    ss << "soundaliases/" << assetName << ".vfcurve";

    return ss.str();
}

bool AssetDumperSndCurve::ShouldDump(XAssetInfo<SndCurve>* asset)
{
    return true;
}

void AssetDumperSndCurve::DumpAsset(AssetDumpingContext& context, XAssetInfo<SndCurve>* asset)
{
    const auto* sndCurve = asset->Asset();

    const auto assetFile = context.OpenAssetFile(GetAssetFilename(sndCurve->filename));

    if (!assetFile)
        return;

    SndCurveDumper dumper(*assetFile);

    const auto knotCount = std::min(static_cast<size_t>(sndCurve->knotCount), std::extent_v<decltype(SndCurve::knots)>);
    dumper.Init(knotCount);

    for (auto i = 0u; i < knotCount; i++)
        dumper.WriteKnot(sndCurve->knots[i][0], sndCurve->knots[i][1]);
}
