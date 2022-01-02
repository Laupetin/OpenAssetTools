#include "AssetLoaderSndCurve.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderSndCurve::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* sndCurve = memory->Create<SndCurve>();
    memset(sndCurve, 0, sizeof(SndCurve));
    sndCurve->filename = memory->Dup(assetName.c_str());
    return sndCurve;
}
