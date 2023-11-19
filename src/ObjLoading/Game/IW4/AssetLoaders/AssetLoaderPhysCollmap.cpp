#include "AssetLoaderPhysCollmap.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderPhysCollmap::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* collmap = memory->Create<PhysCollmap>();
    memset(collmap, 0, sizeof(PhysCollmap));
    collmap->name = memory->Dup(assetName.c_str());
    return collmap;
}
