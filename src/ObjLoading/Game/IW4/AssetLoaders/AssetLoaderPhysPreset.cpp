#include "AssetLoaderPhysPreset.h"

#include <cstring>

#include "ObjLoading.h"
#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

void* AssetLoaderPhysPreset::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* physPreset = memory->Create<PhysPreset>();
    memset(physPreset, 0, sizeof(PhysPreset));
    physPreset->name = memory->Dup(assetName.c_str());
    return physPreset;
}
