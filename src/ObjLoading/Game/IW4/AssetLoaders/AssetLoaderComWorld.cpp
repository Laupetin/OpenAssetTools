#include "AssetLoaderComWorld.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderComWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* comWorld = memory->Create<ComWorld>();
    memset(comWorld, 0, sizeof(ComWorld));
    comWorld->name = memory->Dup(assetName.c_str());
    return comWorld;
}
