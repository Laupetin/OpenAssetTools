#include "AssetLoaderFxWorld.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderFxWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* fxWorld = memory->Create<FxWorld>();
    memset(fxWorld, 0, sizeof(FxWorld));
    fxWorld->name = memory->Dup(assetName.c_str());
    return fxWorld;
}
