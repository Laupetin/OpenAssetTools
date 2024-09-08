#include "AssetLoaderMenu.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderMenu::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetMenu::Type>();
    asset->window.name = memory->Dup(assetName.c_str());
    return asset;
}
