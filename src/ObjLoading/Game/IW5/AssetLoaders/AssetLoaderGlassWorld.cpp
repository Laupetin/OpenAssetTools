#include "AssetLoaderGlassWorld.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderGlassWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* glassWorld = memory->Create<GlassWorld>();
    memset(glassWorld, 0, sizeof(GlassWorld));
    glassWorld->name = memory->Dup(assetName.c_str());
    return glassWorld;
}
