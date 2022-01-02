#include "AssetLoaderClipMap.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderClipMap::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* clipMap = memory->Create<clipMap_t>();
    memset(clipMap, 0, sizeof(clipMap_t));
    clipMap->name = memory->Dup(assetName.c_str());
    return clipMap;
}

asset_type_t AssetLoaderClipMapSp::GetHandlingAssetType() const
{
    return ASSET_TYPE_CLIPMAP_SP;
}

asset_type_t AssetLoaderClipMapMp::GetHandlingAssetType() const
{
    return ASSET_TYPE_CLIPMAP_MP;
}
