#include "AssetLoaderImpactFx.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderImpactFx::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetImpactFx::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
