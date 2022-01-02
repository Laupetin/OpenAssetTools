#include "AssetLoaderPhysCollmap.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderPhysCollmap::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* collmap = memory->Create<PhysCollmap>();
    memset(collmap, 0, sizeof(PhysCollmap));
    collmap->name = memory->Dup(assetName.c_str());
    return collmap;
}
