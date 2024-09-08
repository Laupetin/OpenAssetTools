#include "AssetLoaderSkinnedVerts.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderSkinnedVerts::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetSkinnedVerts::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
