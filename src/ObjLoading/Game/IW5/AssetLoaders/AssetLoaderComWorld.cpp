#include "AssetLoaderComWorld.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderComWorld::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* comWorld = memory->Create<ComWorld>();
    memset(comWorld, 0, sizeof(ComWorld));
    comWorld->name = memory->Dup(assetName.c_str());
    return comWorld;
}
