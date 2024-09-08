#include "AssetLoaderSoundPatch.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderSoundPatch::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetSoundPatch::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
