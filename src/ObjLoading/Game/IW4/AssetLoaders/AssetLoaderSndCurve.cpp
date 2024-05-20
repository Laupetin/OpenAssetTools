#include "AssetLoaderSndCurve.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"
#include "Sound/SoundCurveLoader.h"

#include <cstring>
#include <iostream>
#include <sstream>

using namespace IW4;

void* AssetLoaderSndCurve::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* sndCurve = memory->Create<SndCurve>();
    memset(sndCurve, 0, sizeof(SndCurve));
    sndCurve->filename = memory->Dup(assetName.c_str());
    return sndCurve;
}

bool AssetLoaderSndCurve::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderSndCurve::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto sndCurveData = sound_curve::LoadSoundCurve(manager, assetName);

    if (!sndCurveData)
        return false;

    if (sndCurveData->knots.size() > std::extent_v<decltype(SndCurve::knots)>)
    {
        std::cerr << "Failed to load SndCurve \"" << assetName << "\": Too many knots (" << sndCurveData->knots.size() << ")\n";
        return false;
    }

    auto* sndCurve = memory->Create<SndCurve>();
    sndCurve->filename = memory->Dup(assetName.c_str());
    sndCurve->knotCount = static_cast<uint16_t>(sndCurveData->knots.size());

    for (auto i = 0u; i < std::extent_v<decltype(SndCurve::knots)>; i++)
    {
        if (i < sndCurveData->knots.size())
        {
            const auto& [x, y] = sndCurveData->knots[i];
            sndCurve->knots[i][0] = static_cast<float>(x);
            sndCurve->knots[i][1] = static_cast<float>(y);
        }
        else
        {
            sndCurve->knots[i][0] = 0;
            sndCurve->knots[i][1] = 0;
        }
    }

    manager->AddAsset<AssetSoundCurve>(assetName, sndCurve);

    return true;
}
