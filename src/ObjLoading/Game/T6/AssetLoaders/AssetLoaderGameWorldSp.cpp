#include "AssetLoaderGameWorldSp.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderGameWorldSp::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetGameWorldSp::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
