#include "AssetLoaderSndCurve.h"

#include "AssetLoading/SndCurve/SndCurveReader.h"
#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <iostream>
#include <sstream>

using namespace IW4;

std::string AssetLoaderSndCurve::GetAssetFilename(const std::string& assetName)
{
    std::ostringstream ss;

    ss << "soundaliases/" << assetName << ".vfcurve";

    return ss.str();
}

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
    const auto filename = GetAssetFilename(assetName);
    const auto file = searchPath->Open(filename);
    if (!file.IsOpen())
        return false;

    const SndCurveReader reader(*file.m_stream, filename);

    const auto sndCurveData = reader.Read();

    if (!sndCurveData)
        return false;

    if (sndCurveData->m_knots.size() > std::extent_v<decltype(SndCurve::knots)>)
    {
        std::cerr << "Failed to load SndCurve \"" << assetName << "\": Too many knots (" << sndCurveData->m_knots.size() << ")\n";
        return false;
    }

    auto* sndCurve = memory->Create<SndCurve>();
    sndCurve->filename = memory->Dup(assetName.c_str());
    sndCurve->knotCount = static_cast<uint16_t>(sndCurveData->m_knots.size());

    for (auto i = 0u; i < std::extent_v<decltype(SndCurve::knots)>; i++)
    {
        if (i < sndCurveData->m_knots.size())
        {
            const auto& [x, y] = sndCurveData->m_knots[i];
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
