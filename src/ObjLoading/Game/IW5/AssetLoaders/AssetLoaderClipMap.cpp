#include "AssetLoaderClipMap.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderClipMap::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* clipMap = memory->Create<clipMap_t>();
    memset(clipMap, 0, sizeof(clipMap_t));
    clipMap->name = memory->Dup(assetName.c_str());
    return clipMap;
}
