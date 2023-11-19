#include "AssetLoaderMapEnts.h"

#include "Game/IW4/IW4.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW4;

void* AssetLoaderMapEnts::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* mapEnts = memory->Create<MapEnts>();
    memset(mapEnts, 0, sizeof(MapEnts));
    mapEnts->name = memory->Dup(assetName.c_str());
    return mapEnts;
}
