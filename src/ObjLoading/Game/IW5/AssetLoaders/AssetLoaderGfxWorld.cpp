#include "AssetLoaderGfxWorld.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderGfxWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* gfxWorld = memory->Create<GfxWorld>();
    memset(gfxWorld, 0, sizeof(GfxWorld));
    gfxWorld->name = memory->Dup(assetName.c_str());
    return gfxWorld;
}
