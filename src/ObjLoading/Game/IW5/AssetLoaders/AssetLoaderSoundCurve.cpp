#include "AssetLoaderSoundCurve.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderSoundCurve::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* sndCurve = memory->Create<SndCurve>();
    memset(sndCurve, 0, sizeof(SndCurve));
    sndCurve->filename = memory->Dup(assetName.c_str());
    return sndCurve;
}
