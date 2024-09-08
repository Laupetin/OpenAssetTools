#include "AssetLoaderGameWorldMp.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderGameWorldMp::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetGameWorldMp::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
