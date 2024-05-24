#include "AssetLoaderPhysPreset.h"

#include "Game/IW5/IW5.h"
#include "ObjLoading.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>

using namespace IW5;

void* AssetLoaderPhysPreset::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* physPreset = memory->Create<PhysPreset>();
    memset(physPreset, 0, sizeof(PhysPreset));
    physPreset->name = memory->Dup(assetName.c_str());
    return physPreset;
}
