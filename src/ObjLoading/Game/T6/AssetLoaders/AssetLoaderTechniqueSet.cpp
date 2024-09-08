#include "AssetLoaderTechniqueSet.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderTechniqueSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetTechniqueSet::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
