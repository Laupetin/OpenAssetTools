#include "AssetLoaderEmblemSet.h"

#include "Game/T6/T6.h"
#include "ObjLoading.h"

using namespace T6;

void* AssetLoaderEmblemSet::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    // Asset does not have a name
    // So this cannot be an asset reference
    return nullptr;
}
