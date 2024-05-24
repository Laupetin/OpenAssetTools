#include "AssetLoaderPhysCollmap.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderPhysCollmap::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* collmap = memory->Create<PhysCollmap>();
    memset(collmap, 0, sizeof(PhysCollmap));
    collmap->name = memory->Dup(assetName.c_str());
    return collmap;
}
