#include "AssetLoaderFont.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderFont::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* asset = memory->Alloc<AssetFont::Type>();
    asset->fontName = memory->Dup(assetName.c_str());
    return asset;
}
