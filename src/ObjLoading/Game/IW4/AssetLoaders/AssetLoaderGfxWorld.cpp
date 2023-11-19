#include "AssetLoaderGfxWorld.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderGfxWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* gfxWorld = memory->Create<GfxWorld>();
    memset(gfxWorld, 0, sizeof(GfxWorld));
    gfxWorld->name = memory->Dup(assetName.c_str());
    return gfxWorld;
}
