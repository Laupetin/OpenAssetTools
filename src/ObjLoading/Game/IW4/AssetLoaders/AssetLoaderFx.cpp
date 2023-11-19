#include "AssetLoaderFx.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderFx::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* fx = memory->Create<FxEffectDef>();
    memset(fx, 0, sizeof(FxEffectDef));
    fx->name = memory->Dup(assetName.c_str());
    return fx;
}
