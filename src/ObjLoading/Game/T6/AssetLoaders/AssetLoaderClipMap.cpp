#include "AssetLoaderClipMap.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderClipMapBase::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<clipMap_t>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}

asset_type_t AssetLoaderClipMap::GetHandlingAssetType() const
{
    return ASSET_TYPE_CLIPMAP;
}

asset_type_t AssetLoaderClipMapPvs::GetHandlingAssetType() const
{
    return ASSET_TYPE_CLIPMAP_PVS;
}
