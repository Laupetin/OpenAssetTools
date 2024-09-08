#include "AssetLoaderGfxLightDef.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderGfxLightDef::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetLightDef::Type>();
    asset->name = memory->Dup(assetName.c_str());
    return asset;
}
